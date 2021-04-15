////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Implementation of Oboe Backend Sound Playing Context
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeSoundPlayingContext.h"

// Standard Library
#include <cstdint>
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Sound
#include "../Provider/ISoundProvider.h"
#include "OboeSoundMixer.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			//////////////////////////////////////////////////////////////////////////////// Code-Level Settings

			namespace _Internal
			{
				constexpr int kDataBufferPrepareCount = 4;
			}


			//////////////////////////////////////////////////////////////////////////////// OboeSoundPlayingContext

			//------------------------------------------------------------------------------
			OboeSoundPlayingContext::OboeSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, bool isStartImmediately, float volume, bool isPauseAfterFinish)
				: m_mixerHolder(mixerHolder)
				, m_soundProvider(soundProvider), m_soundFormat(soundProvider->GetFormat())
				, m_soundChannels(static_cast<int32_t>(GetChannelNumber(soundProvider->GetFormat()))), m_soundSampleFrequency(static_cast<int32_t>(soundProvider->GetFrequency())), m_soundSampleByteSize(GetSampleByteSize(soundProvider->GetFormat()))
				, m_isPaused(!isStartImmediately), m_isPlayFinished(false), m_isAllDataQueued(false), m_isAllDataStreamed(false), m_lastSampleNumber(0), m_isPauseAfterFinish(isPauseAfterFinish)
				, m_volume(std::isfinite(volume) ? volume : 0.0f)
				, m_bufferSize(GetDataBufferDefaultSize()), m_lastQueuedBufferIndex(-1)
				, m_bufferLock(false), m_currentBufferLock(false)
			{
				m_samplesPerDataBuffer = static_cast<int>(m_bufferSize / m_soundSampleByteSize);

				_PrapareDataBuffer();
			}

			//------------------------------------------------------------------------------
			OboeSoundPlayingContext::~OboeSoundPlayingContext()
			{
				if (m_currentBuffer)
				{
					_ReleaseBuffer(m_currentBuffer);
				}
				for (int i = 0; i < m_bufferQueue.size(); ++i)
				{
					_ReleaseBuffer(std::get<2>(m_bufferQueue[i]));
				}
				_FlushFreeBuffers();
			}

			//------------------------------------------------------------------------------
			bool OboeSoundPlayingContext::IsFinished() const
			{
				return m_isPlayFinished;
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::Stop()
			{
				if (m_isPlayFinished)
				{
					return;
				}

				m_isPlayFinished = true;
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::Pause()
			{
				m_isPaused = true;
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::Resume()
			{
				m_isPaused = false;

				if (m_isPlayFinished)
				{
					return;
				}

				_PrapareDataBuffer();
				
				std::shared_ptr<Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
				OboeSoundMixer *mixer = nullptr;
				std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
				if (ownerMixerHolder)
				{
					lockContext = ownerMixerHolder->LockTarget();
					if (lockContext)
					{
						mixer = lockContext->GetTypedObject<OboeSoundMixer>();
					}
				}
				if (mixer != nullptr)
				{
					mixer->_NotifyPlayingSoundResume(shared_from_this());
				}
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::_Update()
			{
				_FlushFreeBuffers();

				if (m_isPlayFinished)
				{
					return;
				}

				if (!m_isAllDataStreamed)
				{
					_PrapareDataBuffer();
				}
				else
				{
					if (m_isPauseAfterFinish)
					{
						m_isPaused = true;
					}
					else
					{
						m_isPlayFinished = true;
					}
				}
			}

			//------------------------------------------------------------------------------
			// Set playing position in sample unit
			bool OboeSoundPlayingContext::Seek(SampleNumberType samplePosition)
			{
				if (m_isPlayFinished)
				{
					return false;
				}

				// Prepare Buffers
				int sampleBufferIndex = static_cast<int>(samplePosition / m_samplesPerDataBuffer);
				int sampleIndex = static_cast<int>(samplePosition % m_samplesPerDataBuffer);

				bool isAllDataFilled = false;

				std::vector<std::tuple<int, size_t, std::shared_ptr<DataBufferType> > > preparedBuffers;
				preparedBuffers.reserve(_Internal::kDataBufferPrepareCount + 1);
				for (int i = 0; i < _Internal::kDataBufferPrepareCount + 1; ++i)
				{
					std::shared_ptr<DataBufferType> buffer = _RetainBuffer();
					if (!buffer)
					{
						// Buffer Allocation FAILED!!
						isAllDataFilled = true;
						break;
					}
					buffer->resize(m_bufferSize);

					int fillingBufferIndex = sampleBufferIndex + i;
					size_t filledSize = _FillBuffer(fillingBufferIndex, *buffer);

					preparedBuffers.push_back(std::make_tuple(fillingBufferIndex, filledSize, buffer));

					if (filledSize < m_bufferSize)
					{
						isAllDataFilled = true;
						break;
					}
				}

				if (std::get<1>(preparedBuffers.front()) <= sampleIndex)
				{
					// Over Sound Range
					_ReleaseBuffer(std::get<2>(preparedBuffers.front()));
					Stop();
					m_isAllDataQueued = true;
					m_lastSampleNumber = samplePosition;
					return false;
				}

				std::shared_ptr<DataBufferType> currentBufferCopy;

				int preparedLastBufferIndex = std::get<0>(preparedBuffers.back());

				std::tuple<int, size_t, std::shared_ptr<DataBufferType> > toBeCurrentBuffer = preparedBuffers.front();
				preparedBuffers.erase(preparedBuffers.begin());

				// Swap State in Lock
				{
					// Lock Current Buffer
					bool isCurBufferLocked = false;
					while (!m_currentBufferLock.compare_exchange_strong(isCurBufferLocked, true))
					{
						// Hot Waiting to non-blocking
					}
					auto releaseCurrentLockFunc = [this]() mutable { m_currentBufferLock.store(false); };
					FINALLY_OF_BLOCK(_RELEASE_CUR_LOCK, releaseCurrentLockFunc);

					// Lock Buffers
					bool isBufferLocked = false;
					while (!m_bufferLock.compare_exchange_strong(isBufferLocked, true))
					{
						// Hot Waiting to non-blocking
					}
					auto releaseLockFunc = [this]() mutable { m_bufferLock.store(false); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					// Swap Buffer State
					currentBufferCopy = m_currentBuffer;
					m_currentBuffer = std::get<2>(toBeCurrentBuffer);
					m_currentBufferFilledSize = std::get<1>(toBeCurrentBuffer);
					m_currentBufferReadSample = sampleIndex;

					m_bufferQueue.swap(preparedBuffers);
					m_lastQueuedBufferIndex = preparedLastBufferIndex;

					m_isAllDataQueued = isAllDataFilled;
					m_lastSampleNumber = samplePosition;
				}

				// Release Old Buffers
				if (currentBufferCopy)
				{
					_ReleaseBuffer(currentBufferCopy);
				}
				for (int i = 0; i < preparedBuffers.size(); ++i)
				{
					_ReleaseBuffer(std::get<2>(preparedBuffers[i]));
				}

				return true;
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::SetVolume(float volume)
			{
				if (!std::isfinite(volume))
				{
					volume = 0.0f;
				}
				m_volume.store(volume);
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::_PrapareDataBuffer()
			{
				constexpr int kMaxErrorRetryCount = 3;

				int errorRetryCount = 0;
				while (m_bufferQueue.size() < _Internal::kDataBufferPrepareCount)
				{
					if (m_isAllDataQueued)
					{
						break;
					}

					auto lockContext = m_bufferQueueTailLock.Lock();
					if (!lockContext)
					{
						// Something Wrong
						++errorRetryCount;
						if (errorRetryCount > kMaxErrorRetryCount)
						{
							break;
						}
						else
						{
							continue;
						}
					}

					std::shared_ptr<DataBufferType> buffer = _RetainBuffer();
					if (!buffer)
					{
						// Buffer Allocation FAILED!!
						m_isAllDataQueued = true;
						break;
					}
					buffer->resize(m_bufferSize);

					size_t bufferFilledSize = _FillBuffer(m_lastQueuedBufferIndex + 1, *buffer);

					bool isBufferLocked = false;
					while (!m_bufferLock.compare_exchange_strong(isBufferLocked, true))
					{
						sched_yield();
					}
					{
						auto releaseLockFunc = [this]() mutable { m_bufferLock.store(false); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						++m_lastQueuedBufferIndex;
						m_bufferQueue.push_back(std::make_tuple(m_lastQueuedBufferIndex, bufferFilledSize, buffer));
					}

					if (bufferFilledSize < m_bufferSize)
					{
						m_isAllDataQueued = true;
					}
				}
			}

			//------------------------------------------------------------------------------
			size_t OboeSoundPlayingContext::_FillBuffer(int bufferIndex, DataBufferType &buffer)
			{
				const size_t bufferSize = buffer.size();
				size_t remainBufferSize = bufferSize;
				uint8_t *bufferPtr = (uint8_t *)&buffer[0];
				SampleNumberType currentSamplePointer = static_cast<SampleNumberType>(bufferIndex) * static_cast<SampleNumberType>(m_samplesPerDataBuffer);

				while (remainBufferSize > 0)
				{
					SampleNumberType readSamples = 0;
					size_t readDataBytes = m_soundProvider->FillSampleData(bufferPtr, remainBufferSize, currentSamplePointer, &readSamples);
					remainBufferSize -= readDataBytes;
					currentSamplePointer += readSamples;
					if (readDataBytes == 0)
					{
						// End of Stream
						break;
					}
				}

				return bufferSize - remainBufferSize;
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::FillStreamData(void *dataBuffer, void *targetBuffer, int frameCount, bool isApplyVolume, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount)
			{
				constexpr float kVolumeEpsilon = 0.000001f;
				float currentVolume = 0.0f;

				if (isApplyVolume)
				{
					currentVolume = m_volume.load();
					if (currentVolume > -kVolumeEpsilon && currentVolume < kVolumeEpsilon)
					{
						// Zero Volume
						if (streamDataFormat == oboe::AudioFormat::Float)
						{
							size_t fillingValues = static_cast<size_t>(frameCount * streamChannelCount);
							float *typedAudioDataBuffer = (float *)targetBuffer;
							for (size_t i = 0; i < fillingValues; ++i)
							{
								typedAudioDataBuffer[i] = 0.0f;
							}
						}
						else
						{
							memset(targetBuffer, 0, streamDataFrameBytes * frameCount);
						}
						return;
					}
				}

				// For optimization, implemented this function for case by case
				if (streamDataFormat == oboe::AudioFormat::I16)
				{
					int16_t *typedTargetBuffer = (int16_t *)targetBuffer;

					if (m_soundFormat == SampleFormat::kMono_16 || m_soundFormat == SampleFormat::kStereo_16)
					{
						int16_t *typedDataBuffer = (int16_t *)dataBuffer;

						if (streamChannelCount == m_soundChannels)
						{
							// Same Data Format
							memcpy(targetBuffer, dataBuffer, frameCount * streamDataFrameBytes);
						}
						else if (streamChannelCount == 2 && m_soundChannels == 1)
						{
							// Same format, but multiplex
							for (int i = 0; i < frameCount; ++i)
							{
								*(typedTargetBuffer + i * 2) = *(typedDataBuffer + i);
								*(typedTargetBuffer + i * 2 + 1) = *(typedDataBuffer + i);
							}
						}
						else if (streamChannelCount == 1 && m_soundChannels == 2)
						{
							// Same format, but mix
							for (int i = 0; i < frameCount; ++i)
							{
								// just integer averaging
								*(typedTargetBuffer + i) = static_cast<int16_t>((static_cast<int32_t>(*(typedDataBuffer + i * 2)) + static_cast<int32_t>(*(typedDataBuffer + i * 2 + 1))) / 2);
							}
						}
					}
					else if (m_soundFormat == SampleFormat::kMono_8)
					{
						uint8_t *typedDataBuffer = (uint8_t *)dataBuffer;
						if (streamChannelCount == 2)
						{
							// multiplex 8-bit to 16-bit
							for (int i = 0; i < frameCount; ++i)
							{
								int16_t sampleValue = static_cast<int16_t>((static_cast<int32_t>(*(typedDataBuffer + i)) - 128) * 256);
								*(typedTargetBuffer + i * 2) = sampleValue;
								*(typedTargetBuffer + i * 2 + 1) = sampleValue;
							}
						}
						else if (streamChannelCount == 1)
						{
							// same channel 8-bit to 16-bit
							for (int i = 0; i < frameCount; ++i)
							{
								int16_t sampleValue = static_cast<int16_t>((static_cast<int32_t>(*(typedDataBuffer + i)) - 128) * 256);
								*(typedTargetBuffer + i) = sampleValue;
							}
						}
					}
					else if (m_soundFormat == SampleFormat::kStereo_8)
					{
						uint8_t *typedDataBuffer = (uint8_t *)dataBuffer;
						if (streamChannelCount == 2)
						{
							// same channel 8-bit to 16-bit
							for (int i = 0; i < frameCount * 2; ++i)
							{
								*(typedTargetBuffer + i) = static_cast<int16_t>((static_cast<int32_t>(*(typedDataBuffer + i)) - 128) * 256);
							}
						}
						else if (streamChannelCount == 1)
						{
							// mix 8-bit to 16-bit
							for (int i = 0; i < frameCount; ++i)
							{
								int16_t sampleValue = static_cast<int16_t>(((static_cast<int32_t>(*(typedDataBuffer + i * 2)) - 128) + (static_cast<int32_t>(*(typedDataBuffer + i * 2 + 1)) - 128)) * 256 / 2);
								*(typedTargetBuffer + i * 2) = sampleValue;
								*(typedTargetBuffer + i * 2 + 1) = sampleValue;
							}
						}
					}
				}
				else if (streamDataFormat == oboe::AudioFormat::Float)
				{
					float *typedTargetBuffer = (float *)targetBuffer;
					switch (m_soundFormat)
					{
						case SampleFormat::kMono_8:
							{
								uint8_t *typedDataBuffer = (uint8_t *)dataBuffer;
								if (streamChannelCount == 1)
								{
									// same channel 8-bit to float
									for (int i = 0; i < frameCount; ++i)
									{
										*(typedTargetBuffer + i) = (static_cast<int32_t>(*(typedDataBuffer + i)) - 128) / 128.0f;
									}
								}
								else if (streamChannelCount == 2)
								{
									// multiplex 8-bit to float
									for (int i = 0; i < frameCount; ++i)
									{
										float sampleValue = (static_cast<int32_t>(*(typedDataBuffer + i)) - 128) / 128.0f;
										*(typedTargetBuffer + i * 2) = sampleValue;
										*(typedTargetBuffer + i * 2 + 1) = sampleValue;
									}
								}
							}
							break;

						case SampleFormat::kStereo_8:
							{
								uint8_t *typedDataBuffer = (uint8_t *)dataBuffer;
								if (streamChannelCount == 1)
								{
									// mix 8-bit to float
									for (int i = 0; i < frameCount; ++i)
									{
										*(typedTargetBuffer + i) = ((static_cast<int32_t>(*(typedDataBuffer + i * 2)) - 128) + (static_cast<int32_t>(*(typedDataBuffer + i * 2 + 1)) - 128)) / 256.0f;
									}
								}
								else if (streamChannelCount == 2)
								{
									// same channel 8-bit to float
									for (int i = 0; i < frameCount * 2; ++i)
									{
										*(typedTargetBuffer + i) = (static_cast<int32_t>(*(typedDataBuffer + i)) - 128) / 128.0f;
									}
								}
							}
							break;

						case SampleFormat::kMono_16:
							{
								int16_t *typedDataBuffer = (int16_t *)dataBuffer;
								if (streamChannelCount == 1)
								{
									// same channel 16-bit to float
									for (int i = 0; i < frameCount; ++i)
									{
										*(typedTargetBuffer + i) = *(typedDataBuffer + i) / 32768.0f;
									}
								}
								else if (streamChannelCount == 2)
								{
									// multiplex 16-bit to float
									for (int i = 0; i < frameCount; ++i)
									{
										float sampleValue = *(typedDataBuffer + i) / 32768.0f;
										*(typedTargetBuffer + i * 2) = sampleValue;
										*(typedTargetBuffer + i * 2 + 1) = sampleValue;
									}
								}
							}
							break;

						case SampleFormat::kStereo_16:
							{
								int16_t *typedDataBuffer = (int16_t *)dataBuffer;
								if (streamChannelCount == 1)
								{
									// mix 16-bit to float
									for (int i = 0; i < frameCount; ++i)
									{
										*(typedTargetBuffer + i) = (static_cast<int32_t>(*(typedDataBuffer + i * 2)) + static_cast<int32_t>(*(typedDataBuffer + i * 2 + 1))) / 65536.0f;
									}
								}
								else if (streamChannelCount == 2)
								{
									// same channel 16-bit to float
									for (int i = 0; i < frameCount * 2; ++i)
									{
										*(typedTargetBuffer + i) = *(typedDataBuffer + i) / 32768.0f;
									}
								}
							}
							break;

						default:
							// Do Nothing for Invalid Format
							break;
					}
				}

				if (isApplyVolume)
				{
					if (currentVolume < 1.0 - kVolumeEpsilon || currentVolume > 1.0 + kVolumeEpsilon)
					{
						// Non-identical Volume
						int valueCount = frameCount * streamChannelCount;
						if (streamDataFormat == oboe::AudioFormat::I16)
						{
							int16_t *typedTargetBuffer = (int16_t *)targetBuffer;
							for (int i = 0; i < valueCount; ++i)
							{
								typedTargetBuffer[i] = static_cast<int16_t>(Utility::Math::ClipMinMax(static_cast<int32_t>(typedTargetBuffer[i] * currentVolume), -32768, 32767));
							}
						}
						else if (streamChannelCount == 1 && m_soundChannels == 2)
						{
							float *typedTargetBuffer = (float *)targetBuffer;
							for (int i = 0; i < valueCount; ++i)
							{
								typedTargetBuffer[i] = Utility::Math::ClipMinMax(typedTargetBuffer[i] * currentVolume, -1.0f, 1.0f);
							}
						}
					}
				}
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<DataBufferType> OboeSoundPlayingContext::_RetainBuffer()
			{
				std::shared_ptr<DataBufferType> freedBuffer;
				if (m_freedBuffers.try_dequeue(freedBuffer))
				{
					return freedBuffer;
				}

				std::shared_ptr<Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
				if (!ownerMixerHolder)
				{
					return nullptr;
				}
				auto lockContext = ownerMixerHolder->LockTarget();
				if (!lockContext)
				{
					return nullptr;
				}
				OboeSoundMixer *mixer = lockContext->GetTypedObject<OboeSoundMixer>();
				if (mixer == nullptr)
				{
					return nullptr;
				}

				return mixer->m_segmentedBufferPool.GetObject();
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::_ReleaseBuffer(std::shared_ptr<DataBufferType> buffer)
			{
				m_freedBuffers.enqueue(buffer);
			}

			//------------------------------------------------------------------------------
			void OboeSoundPlayingContext::_FlushFreeBuffers()
			{
				std::shared_ptr<Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
				OboeSoundMixer *mixer = nullptr;
				std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
				if (ownerMixerHolder)
				{
					lockContext = ownerMixerHolder->LockTarget();
					if (lockContext)
					{
						mixer = lockContext->GetTypedObject<OboeSoundMixer>();
					}
				}
				
				std::shared_ptr<DataBufferType> freedBuffer;
				while (m_freedBuffers.try_dequeue(freedBuffer))
				{
					if (mixer != nullptr)
					{
						mixer->m_segmentedBufferPool.ReleaseObject(freedBuffer);
					}
				}
			}

			//------------------------------------------------------------------------------
			bool OboeSoundPlayingContext::RenderAudio(void *audioData, int32_t numFrames, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount)
			{
				if (m_isPlayFinished || m_isPaused || m_isAllDataStreamed)
				{
					return false;
				}

				int32_t remainFrameNumber = numFrames;
				uint8_t *currentTargetBufferHead = (uint8_t *)audioData;
				while (remainFrameNumber > 0)
				{
					// Lock Current Buffer
					bool isCurBufferLocked = false;
					while (!m_currentBufferLock.compare_exchange_strong(isCurBufferLocked, true))
					{
						// Hot Waiting to non-blocking
					}
					auto releaseCurrentLockFunc = [this]() mutable { m_currentBufferLock.store(false); };
					FINALLY_OF_BLOCK(_RELEASE_CUR_LOCK, releaseCurrentLockFunc);

					// Move to next buffer
					size_t bufferBytesRead = m_soundSampleByteSize * static_cast<size_t>(m_currentBufferReadSample);
					while (!m_currentBuffer || bufferBytesRead >= m_currentBufferFilledSize)
					{
						m_currentBufferFilledSize = 0;
						if (m_currentBuffer)
						{
							_ReleaseBuffer(m_currentBuffer);
							m_currentBuffer.reset();
						}

						bool isBufferLocked = false;
						while (!m_bufferLock.compare_exchange_strong(isBufferLocked, true))
						{
							// Hot Waiting to non-blocking
						}
						auto releaseLockFunc = [this]() mutable { m_bufferLock.store(false); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						m_currentBufferReadSample = 0;
						bufferBytesRead = 0;
						if (m_bufferQueue.empty())
						{
							if (m_isAllDataQueued)
							{
								// Finished
								m_isAllDataStreamed = true;
							}
							break;
						}
						else
						{
							std::tuple<int, size_t, std::shared_ptr<DataBufferType> > firstBuffer = m_bufferQueue[0];
							m_bufferQueue.erase(m_bufferQueue.begin());

							m_currentBufferFilledSize = std::get<1>(firstBuffer);
							m_currentBuffer = std::get<2>(firstBuffer);
						}
					}

					// Fill with current buffer
					if (m_currentBuffer)
					{
						int writingSamples = static_cast<int>((m_currentBufferFilledSize - bufferBytesRead) / m_soundSampleByteSize);
						if (writingSamples > remainFrameNumber)
						{
							writingSamples = remainFrameNumber;
						}

						FillStreamData(&(*m_currentBuffer)[0] + bufferBytesRead, currentTargetBufferHead, writingSamples, true, streamDataFrameBytes, streamDataFormat, streamChannelCount);

						m_currentBufferReadSample += writingSamples;
						remainFrameNumber -= writingSamples;
						currentTargetBufferHead += (writingSamples * streamDataFrameBytes);

						m_lastSampleNumber += writingSamples;
					}
					else
					{
						// fill 0 for remain buffer
						if (streamDataFormat == oboe::AudioFormat::Float)
						{
							size_t fillingValues = static_cast<size_t>(remainFrameNumber * streamChannelCount);
							float *typedAudioDataBuffer = (float *)currentTargetBufferHead;
							for (size_t i = 0; i < fillingValues; ++i)
							{
								typedAudioDataBuffer[i] = 0.0f;
							}
						}
						else
						{
							memset(currentTargetBufferHead, 0, remainFrameNumber * streamDataFrameBytes);
						}
						remainFrameNumber = 0;
					}
				}

				return true;
			}
		}
	}
}
