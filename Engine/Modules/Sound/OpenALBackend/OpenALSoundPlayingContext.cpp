////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Implementation of OpenAL Backend Sound Playing Context
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OpenALSoundPlayingContext.h"

// Standard Library
#include <cstdint>

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
			//////////////////////////////////////////////////////////////////////////////// Code-Level Settings

			namespace _Internal
			{
				constexpr size_t kMaxBufferSize = 8192;
			}


			//////////////////////////////////////////////////////////////////////////////// OpenALSoundPlayingContext

			//------------------------------------------------------------------------------
			OpenALSoundPlayingContext::OpenALSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, ALuint source, ALuint buffers[], size_t bufferCount, bool isStartImmediately, float volume, bool isPauseAfterFinish)
				: m_soundProvider(soundProvider), m_source(source), m_isSourceReturned(false)
				, m_mixerHolder(mixerHolder)
				, m_isPlayFinished(false), m_isAllDataQueued(false), m_lastSampleNumber(0)
				, m_isPaused(false), m_isPauseAfterFinish(isPauseAfterFinish)
				, m_lastProcessedSamplePosition(0)
				, m_isPlayUpdateStarted(false), m_isSeekApplied(false)
				, m_volume(volume), m_isVolumeChangeRequested(false)
			{
				alSourcef(m_source, AL_PITCH, 1.0f);
				alSourcef(m_source, AL_GAIN, m_volume);
				alSourcei(m_source, AL_LOOPING, AL_FALSE);

				alGetError();

				if (isStartImmediately)
				{
					// Fill Initial Buffers
					uint8_t dataBuffer[_Internal::kMaxBufferSize];
					for (int i = 0; i < bufferCount; ++i)
					{
						if (i >= 3)
						{
							// Queue first 3 buffers during initializing
							m_reservedBuffers.push_back(buffers[i]);
							continue;
						}

						if (m_isAllDataQueued)
						{
							m_reservedBuffers.push_back(buffers[i]);
						}
						else
						{
							SampleNumberType proceedSamples;
							size_t readDataBytes = m_soundProvider->FillSampleData(dataBuffer, OpenALSoundPlayingContext::g_bufferSize, m_lastSampleNumber, &proceedSamples);
							if (readDataBytes == 0)
							{
								// Data End
								m_isAllDataQueued = true;
								m_reservedBuffers.push_back(buffers[i]);
							}
							else
							{
								ALuint bufferCopy = buffers[i];
								alBufferData(bufferCopy, ToOpenALFormatEnum(m_soundProvider->GetFormat()), dataBuffer, (ALsizei)readDataBytes, m_soundProvider->GetFrequency());
								alSourceQueueBuffers(m_source, 1, &bufferCopy);
								if (alGetError() != AL_NO_ERROR)
								{
									// Buffer Error?
									m_reservedBuffers.push_back(buffers[i]);
								}
								else
								{
									m_bufferSamples.push(m_lastSampleNumber + proceedSamples);
								}
							}
							m_lastSampleNumber += proceedSamples;
						}
					}
					m_bufferCount = bufferCount;
					m_isSeekApplied = true;

					// Start Play
					alSourcePlay(m_source);

					m_isPlayUpdateStarted = true;
				}
				else
				{
					m_isPaused = true;

					for (int i = 0; i < bufferCount; ++i)
					{
						m_reservedBuffers.push_back(buffers[i]);
					}
					m_bufferCount = bufferCount;
				}
			}

			//------------------------------------------------------------------------------
			OpenALSoundPlayingContext::~OpenALSoundPlayingContext()
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
				if (mixer != nullptr)
				{
					if (!m_reservedBuffers.empty())
					{
						mixer->_ReleaseBuffers(&m_reservedBuffers[0], m_reservedBuffers.size());
					}

					bool isNotReturned = false;
					if (m_isSourceReturned.compare_exchange_strong(isNotReturned, true))
					{
						mixer->_PushSourceToRelease(m_source);
					}
				}
			}

			//------------------------------------------------------------------------------
			bool OpenALSoundPlayingContext::IsFinished() const
			{
				// If there is no buffer, it means cannot play more due to some errors
				return (m_isPlayFinished || (m_bufferCount == 0));
			}

			//------------------------------------------------------------------------------
			void OpenALSoundPlayingContext::Stop()
			{
				if (m_isPlayFinished)
				{
					return;
				}

				m_isSeekApplied = true;
				m_isAllDataQueued = true;
				//note: can empty already queued buffers for more precise behavior
			}

			//------------------------------------------------------------------------------
			void OpenALSoundPlayingContext::Pause()
			{
				m_isPaused = true;
				//note: can dequeue and store already queued buffers for more precise behavior
			}

			//------------------------------------------------------------------------------
			void OpenALSoundPlayingContext::Resume()
			{
				m_isPlayUpdateStarted = false;
				m_isPaused = false;

				if (!m_isPlayFinished)
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
					if (mixer != nullptr)
					{
						mixer->_NotifyPlayingSoundResume(shared_from_this());
					}
				}
			}

			//------------------------------------------------------------------------------
			void OpenALSoundPlayingContext::_Update(std::vector<ALuint> &releasingBuffers)
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

				ALint sourceState;
				if (m_isAllDataQueued && m_isSeekApplied)
				{
					if (!m_isPauseAfterFinish)
					{
						for (std::vector<ALuint>::iterator it = m_reservedBuffers.begin(); it != m_reservedBuffers.end(); ++it)
						{
							releasingBuffers.push_back(*it);
							--m_bufferCount;
						}
						m_reservedBuffers.clear();
					}

					ALint queuedBufferCount;
					alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);

					// Resource Play State
					alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);

					if (queuedBufferCount <= 0 || sourceState == AL_STOPPED)
					{
						alGetError();
						for (int i = 0; i < queuedBufferCount; ++i)
						{
							ALuint dequeuedBuffer;
							alSourceUnqueueBuffers(m_source, 1, &dequeuedBuffer);
							if (alGetError() == AL_NO_ERROR)
							{
								if (m_isPauseAfterFinish)
								{
									m_reservedBuffers.push_back(dequeuedBuffer);
								}
								else
								{
									releasingBuffers.push_back(dequeuedBuffer);
								}
							}
							m_lastProcessedSamplePosition = m_bufferSamples.front();
							m_bufferSamples.pop();
						}
						if (m_isPauseAfterFinish)
						{
							m_isPaused = true;
						}
						else
						{
							m_isPlayFinished = true;
						}
						alSourceStop(m_source);
						return;
					}

					if (sourceState != AL_PLAYING && !m_isPaused)
					{
						alSourcePlay(m_source);
					}
				}

				ALint processedBuffers;
				alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);
				if (processedBuffers <= 0)
				{
					if (m_reservedBuffers.empty())
					{
						return;
					}
				}

				alGetError();
				for (int i = 0; i < processedBuffers; ++i)
				{
					ALuint dequeuedBuffer;
					alSourceUnqueueBuffers(m_source, 1, &dequeuedBuffer);
					if (alGetError() == AL_NO_ERROR)
					{
						m_reservedBuffers.push_back(dequeuedBuffer);
					}
					m_lastProcessedSamplePosition = m_bufferSamples.front();
					m_bufferSamples.pop();
				}

				if (m_isPaused)
				{
					return;
				}

				alGetError();

				unsigned char dataBuffer[_Internal::kMaxBufferSize];
				std::vector<ALuint> availableBuffers(m_reservedBuffers);
				m_reservedBuffers.clear();
				for (std::vector<ALuint>::iterator it = availableBuffers.begin(); it != availableBuffers.end(); ++it)
				{
					if (m_isPlayFinished)
					{
						releasingBuffers.push_back(*it);
						--m_bufferCount;
					}
					else if (m_isAllDataQueued && m_isSeekApplied)
					{
						if (m_isPauseAfterFinish)
						{
							m_reservedBuffers.push_back(*it);
						}
						else
						{
							releasingBuffers.push_back(*it);
							--m_bufferCount;
						}
					}
					else
					{
						SampleNumberType proceedSamples;
						SampleNumberType lastSampleNumberCopy = m_lastSampleNumber;
						size_t readDataBytes = m_soundProvider->FillSampleData(dataBuffer, OpenALSoundPlayingContext::g_bufferSize, lastSampleNumberCopy, &proceedSamples);
						if (readDataBytes == 0)
						{
							// Data End
							m_isAllDataQueued = true;
							if (m_isPauseAfterFinish)
							{
								m_reservedBuffers.push_back(*it);
							}
							else
							{
								releasingBuffers.push_back(*it);
								--m_bufferCount;
							}
						}
						else
						{
							m_isAllDataQueued = false;
							ALuint bufferCopy = *it;
							alBufferData(bufferCopy, ToOpenALFormatEnum(m_soundProvider->GetFormat()), dataBuffer, (ALsizei)readDataBytes, m_soundProvider->GetFrequency());
							alSourceQueueBuffers(m_source, 1, &bufferCopy);
							if (alGetError() != AL_NO_ERROR)
							{
								// Buffer Error?
								releasingBuffers.push_back(*it);
								--m_bufferCount;
							}
							else
							{
								m_bufferSamples.push(lastSampleNumberCopy + proceedSamples);
							}
						}
						m_lastSampleNumber += proceedSamples;
					}
				}
				m_isSeekApplied = true;

				alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
				if (sourceState != AL_PLAYING)
				{
					alSourcePlay(m_source);
				}

				m_isPlayUpdateStarted = true;
			}

			//------------------------------------------------------------------------------
			// Function to stop usage of all buffers, when some error occured in creation
			// It should be called only by OpenALSoundMixer friend class, during creation
			void OpenALSoundPlayingContext::_CancelBufferUsage()
			{
				alSourceStop(m_source);

				alGetError();

				ALint queuedBufferCount;
				alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBufferCount);
				for (int i = 0; i < queuedBufferCount; ++i)
				{
					ALuint dequeuedBuffer;
					alSourceUnqueueBuffers(m_source, 1, &dequeuedBuffer);
					if (alGetError() == AL_NO_ERROR)
					{
						m_reservedBuffers.push_back(dequeuedBuffer);
					}
				}
				while (!m_bufferSamples.empty())
				{
					m_bufferSamples.pop();
				}

				m_isPlayFinished = true;
			}

			//------------------------------------------------------------------------------
			// Set playing position in sample unit
			bool OpenALSoundPlayingContext::Seek(SampleNumberType samplePosition)
			{
				m_lastSampleNumber = samplePosition;
				m_isSeekApplied = false;
				return true;
			}

			//------------------------------------------------------------------------------
			void OpenALSoundPlayingContext::SetVolume(float volume)
			{
				m_volume = volume;
				m_isVolumeChangeRequested = true;
			}
		}
	}
}
