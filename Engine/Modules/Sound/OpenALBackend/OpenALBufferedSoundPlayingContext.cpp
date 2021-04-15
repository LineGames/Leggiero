////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALBufferedSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// All data buffered Sound Playing Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OpenALBufferedSoundPlayingContext.h"

// Standard Library
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Sound
#include "../Provider/ISoundProvider.h"
#include "OpenALCommon.h"
#include "OpenALSoundMixer.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			//////////////////////////////////////////////////////////////////////////////// OpenALBufferedSoundPlayingContext

			//------------------------------------------------------------------------------
			OpenALBufferedSoundPlayingContext::OpenALBufferedSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, ALuint source, ALuint buffer, bool isStartImmediately, float volume, bool isLooping, bool isPauseAfterFinish)
				: m_source(source), m_buffer(buffer), m_isSourceReturned(false)
				, m_mixerHolder(mixerHolder)
				, m_isPlayFinished(false), m_isPaused(false), m_isPausedBySystem(false), m_isPauseAfterFinish(isPauseAfterFinish)
				, m_volume(volume), m_isVolumeChangeRequested(false)
			{
				alSourceRewind(m_source);
				alSourcei(m_source, AL_BUFFER, AL_NONE);

				alSourcef(m_source, AL_PITCH, 1.0f);
				alSourcef(m_source, AL_GAIN, m_volume);
				alSourcei(m_source, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);

				_FillBuffer(soundProvider);
				alSourcei(m_source, AL_BUFFER, m_buffer);

				if (isStartImmediately)
				{
					alSourcePlay(m_source);
				}
				else
				{
					m_isPaused.store(true);
				}
			}

			//------------------------------------------------------------------------------
			OpenALBufferedSoundPlayingContext::~OpenALBufferedSoundPlayingContext()
			{
				std::shared_ptr<Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
				OpenALSoundMixer *mixer = nullptr;
				std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
				if (ownerMixerHolder)
				{
					lockContext = ownerMixerHolder->LockTarget();
					if (lockContext)
					{
						mixer = lockContext->GetTypedObject<OpenALSoundMixer>();
					}
				}
				if (mixer == nullptr)
				{
					// Sound System Already Finalized
					return;
				}

				bool isNotReturned = false;
				if (m_isSourceReturned.compare_exchange_strong(isNotReturned, true))
				{
					alSourceStop(m_source);
					mixer->_ReleaseSource(m_source);
				}
				
				mixer->_ReleaseBuffers(&m_buffer, 1);
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::_FillBuffer(std::shared_ptr<ISoundProvider> &soundProvider)
			{
				m_sampleSize = soundProvider->GetSampleLength();

				// General Sound Provider
				{
					size_t sampleUnitSize = GetSampleByteSize(soundProvider->GetFormat());
					
					size_t sampleDataSize = static_cast<size_t>(sampleUnitSize * m_sampleSize);
					if (sampleDataSize == 0)
					{
						return;
					}

					ALvoid *pcmData = (ALvoid *)malloc(sampleDataSize);
					if (pcmData == nullptr)
					{
						// Out of memory
						return;
					}

					// Buffer Auto-Free Block
					{
						auto bufferFreeFunc = [pcmData]() mutable {
							free(pcmData);
						};
						FINALLY_OF_BLOCK(_FREE_DATA_BUFFER, bufferFreeFunc);

						SampleNumberType copiedSample = 0;
						SampleNumberType writtenSampleCountBuffer;
						size_t readOffset = 0;
						size_t readBytes;
						while ((readBytes = soundProvider->FillSampleData((ALvoid *)((unsigned char *)pcmData + readOffset), sampleDataSize - readOffset, copiedSample, &writtenSampleCountBuffer)) > 0)
						{
							copiedSample += writtenSampleCountBuffer;
							readOffset += readBytes;
							if (readOffset >= sampleDataSize)
							{
								break;
							}
						}
						if (copiedSample == 0)
						{
							// Data Error?
							return;
						}

						if (copiedSample != m_sampleSize)
						{
							m_sampleSize = (size_t)copiedSample;
							sampleDataSize = (size_t)(sampleUnitSize * m_sampleSize);
						}

						alBufferData(m_buffer, ToOpenALFormatEnum(soundProvider->GetFormat()), pcmData, static_cast<ALsizei>(sampleDataSize), soundProvider->GetFrequency());
					}
				}
			}

			//------------------------------------------------------------------------------
			bool OpenALBufferedSoundPlayingContext::IsFinished() const
			{
				if (m_isPlayFinished)
				{
					return true;
				}

				if (!m_isPauseAfterFinish)
				{
					ALint sourceState;
					alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
					if (sourceState == AL_STOPPED && !m_isPausedBySystem.load())
					{
						return true;
					}
				}

				return m_isPlayFinished;
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::Stop()
			{
				alSourceStop(m_source);
				m_isPlayFinished = true;
			}

			//------------------------------------------------------------------------------
			bool OpenALBufferedSoundPlayingContext::IsPaused() const
			{
				if (m_isPaused.load())
				{
					return true;
				}

				ALint sourceState;
				alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
				if (sourceState == AL_PAUSED)
				{
					return true;
				}
				if (m_isPauseAfterFinish && sourceState == AL_STOPPED)
				{
					return true;
				}

				return m_isPaused.load();
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::Pause()
			{
				if (!m_isPausedBySystem.load())
				{
					alSourcePause(m_source);
				}
				m_isPaused.store(true);
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::Resume()
			{
				if (m_isPlayFinished)
				{
					// Not in valid state after stopped
					return;
				}

				bool isNeedToResume = false;

				int lockResult = pthread_mutex_lock(&m_pauseResumeCheckMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_pauseResumeCheckMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					if (m_isPaused.load())
					{
						m_isPaused.store(false);
						if (!m_isPausedBySystem.load())
						{
							isNeedToResume = true;
						}
					}
				}
				else
				{
					bool expectedValue = true;
					if (m_isPaused.compare_exchange_strong(expectedValue, false))
					{
						if (!m_isPausedBySystem.load())
						{
							isNeedToResume = true;
						}
					}
				}

				if (isNeedToResume)
				{
					if (IsFinished())
					{
						return;
					}

					std::shared_ptr<Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
					OpenALSoundMixer *mixer = nullptr;
					std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
					if (ownerMixerHolder)
					{
						lockContext = ownerMixerHolder->LockTarget();
						if (lockContext)
						{
							mixer = lockContext->GetTypedObject<OpenALSoundMixer>();
						}
					}
					if (mixer != nullptr)
					{
						mixer->_NotifyPlayingBufferedSoundResume(shared_from_this());
					}

					ALint queuedBufferCount;
					alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);
					if (queuedBufferCount == 0)
					{
						ALint currentBuffer;
						alGetSourcei(m_source, AL_BUFFER, &currentBuffer);
						if (currentBuffer == AL_NONE)
						{
							alSourceRewind(m_source);
							alSourcei(m_source, AL_BUFFER, m_buffer);
							alSourcei(m_source, AL_SAMPLE_OFFSET, (ALint)m_sampleSize);
						}
					}

					alSourcePlay(m_source);
				}
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::SetIsLooping(bool isLooping)
			{
				alSourcei(m_source, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
			}

			//------------------------------------------------------------------------------
			bool OpenALBufferedSoundPlayingContext::IsLooping()
			{
				ALint loopingState;
				alGetSourcei(m_source, AL_LOOPING, &loopingState);

				return (loopingState == AL_TRUE);
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::Rewind()
			{
				if (m_isPlayFinished)
				{
					// Not in valid state after stopped
					return;
				}

				alSourceRewind(m_source);
				alSourcei(m_source, AL_BUFFER, AL_NONE);

				m_isPlayFinished = false;
				alSourcei(m_source, AL_BUFFER, m_buffer);

				m_isPaused.store(true);
			}

			//------------------------------------------------------------------------------
			// Update sound play state
			void OpenALBufferedSoundPlayingContext::_Update()
			{
				if (m_isPlayFinished)
				{
					return;
				}

				if (m_isVolumeChangeRequested)
				{
					alSourcef(m_source, AL_GAIN, m_volume);
					m_isVolumeChangeRequested = false;
				}

				// Resource Play State
				ALint sourceState;
				alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);

				switch (sourceState)
				{
					case AL_STOPPED:
						if (m_isPauseAfterFinish)
						{
							m_isPlayFinished = false;
							m_isPaused = true;
						}
						else
						{
							m_isPlayFinished = true;
						}
						break;

					case AL_PAUSED:
					case AL_INITIAL:
						m_isPlayFinished = false;
						break;

					case AL_PLAYING:
						m_isPlayFinished = false;
						break;
				}
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::_PauseBySystem()
			{
				bool expectedValue = false;
				if (m_isPausedBySystem.compare_exchange_strong(expectedValue, true))
				{
					if (!m_isPaused.load())
					{
						alSourcePause(m_source);
					}
				}
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::_ResumeBySystem()
			{
				bool isNeedToResume = false;

				int lockResult = pthread_mutex_lock(&m_pauseResumeCheckMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_pauseResumeCheckMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					if (m_isPausedBySystem.load())
					{
						m_isPausedBySystem.store(false);
						if (!m_isPaused.load())
						{
							isNeedToResume = true;
						}
					}
				}
				else
				{
					bool expectedValue = true;
					if (m_isPausedBySystem.compare_exchange_strong(expectedValue, false))
					{
						if (!m_isPaused.load())
						{
							isNeedToResume = true;
						}
					}
				}

				if (isNeedToResume)
				{
					if (IsFinished())
					{
						return;
					}

					alSourcePlay(m_source);
				}
			}

			//------------------------------------------------------------------------------
			SampleNumberType OpenALBufferedSoundPlayingContext::GetCurrentPosition()
			{
				if (m_isPlayFinished)
				{
					return m_sampleSize;
				}

				ALint sampleOffset;
				alGetSourcei(m_source, AL_SAMPLE_OFFSET, &sampleOffset);

				return static_cast<SampleNumberType>(sampleOffset);
			}

			//------------------------------------------------------------------------------
			bool OpenALBufferedSoundPlayingContext::Seek(SampleNumberType samplePosition)
			{
				alSourcei(m_source, AL_SAMPLE_OFFSET, (ALint)samplePosition);
				return true;
			}

			//------------------------------------------------------------------------------
			void OpenALBufferedSoundPlayingContext::SetVolume(float volume)
			{
				m_volume = volume;
				m_isVolumeChangeRequested = true;
			}
		}
	}
}
