////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeBufferedSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Implementation of Oboe Backend Sound Playing Context
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeBufferedSoundPlayingContext.h"

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
			//////////////////////////////////////////////////////////////////////////////// OboeBufferedSoundPlayingContext

			//------------------------------------------------------------------------------
			OboeBufferedSoundPlayingContext::OboeBufferedSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, bool isStartImmediately, float volume, bool isLooping, bool isPauseAfterFinish)
				: m_isPaused(!isStartImmediately), m_isPlayFinished(false), m_isAllDataStreamed(false), m_isPauseAfterFinish(isPauseAfterFinish)
				, m_isLooping(isLooping), m_currentReadPosition(0)
				, m_volume(std::isfinite(volume) ? volume : 0.0f)
				, m_dataBuffer(nullptr), m_bufferSampleSize(0)
				, m_currentReadHeaderLock(false)
				, m_mixerHolder(mixerHolder)
			{
				auto lockContext = mixerHolder->LockTarget();
				OboeSoundMixer *mixer = lockContext->GetTypedObject<OboeSoundMixer>();

				_FillBuffer(soundProvider, mixer->m_fullBufferPool);
			}

			//------------------------------------------------------------------------------
			OboeBufferedSoundPlayingContext::~OboeBufferedSoundPlayingContext()
			{
				if (m_dataBufferHolder)
				{
					std::shared_ptr<Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
					if (ownerMixerHolder)
					{
						auto lockContext = ownerMixerHolder->LockTarget();
						if (lockContext)
						{
							OboeSoundMixer *mixer = lockContext->GetTypedObject<OboeSoundMixer>();
							if (mixer != nullptr)
							{
								mixer->m_fullBufferPool.ReleaseObject(m_dataBufferHolder);
							}
						}
					}
				}
			}

			//------------------------------------------------------------------------------
			bool OboeBufferedSoundPlayingContext::IsFinished() const
			{
				return m_isPlayFinished;
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::Stop()
			{
				if (m_isPlayFinished)
				{
					return;
				}
				m_isPlayFinished = true;
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::Pause()
			{
				m_isPaused = true;
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::Resume()
			{
				m_isPaused = false;

				if (m_isPlayFinished)
				{
					return;
				}

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
					mixer->_NotifyPlayingBufferedSoundResume(shared_from_this());
				}
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::_Update()
			{
				if (m_isPlayFinished)
				{
					return;
				}

				if (m_isAllDataStreamed)
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
			bool OboeBufferedSoundPlayingContext::Seek(SampleNumberType samplePosition)
			{
				// Sanitize Input
				if (samplePosition >= m_bufferSampleSize)
				{
					if (m_isLooping)
					{
						samplePosition = samplePosition % m_bufferSampleSize;
					}
					else
					{
						// Invalid Position
						samplePosition = m_bufferSampleSize;
					}
				}

				// Swap State in Lock
				{
					// Lock Current State
					bool isCurLocked = false;
					while (!m_currentReadHeaderLock.compare_exchange_strong(isCurLocked, true))
					{
						// Hot Waiting to non-blocking
					}
					auto releaseCurrentLockFunc = [this]() mutable { m_currentReadHeaderLock.store(false); };
					FINALLY_OF_BLOCK(_RELEASE_CUR_LOCK, releaseCurrentLockFunc);

					// Swap State
					m_isAllDataStreamed = false;
					m_currentReadPosition = samplePosition;
				}

				return true;
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::SetVolume(float volume)
			{
				if (!std::isfinite(volume))
				{
					volume = 0.0f;
				}
				m_volume.store(volume);
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::SetIsLooping(bool isLooping)
			{
				m_isLooping = isLooping;
			}

			//------------------------------------------------------------------------------
			bool OboeBufferedSoundPlayingContext::IsLooping()
			{
				return m_isLooping;
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::Rewind()
			{
				Seek(0);
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::_FillBuffer(std::shared_ptr<ISoundProvider> soundProvider, Utility::Object::VerySimpleObjectPool<DataBufferType> &bufferPool)
			{
				m_soundFormat = soundProvider->GetFormat();
				m_soundChannels = static_cast<int32_t>(GetChannelNumber(soundProvider->GetFormat()));
				m_soundSampleFrequency = static_cast<int32_t>(soundProvider->GetFrequency());
				m_soundSampleByteSize = GetSampleByteSize(soundProvider->GetFormat());

				m_bufferSampleSize = soundProvider->GetSampleLength();

				// General Sound Provider
				{
					size_t sampleDataSize = static_cast<size_t>(m_bufferSampleSize * m_soundSampleByteSize);
					if (sampleDataSize == 0)
					{
						m_bufferSampleSize = 0;
						return;
					}

					m_dataBufferHolder = bufferPool.GetObject();
					m_dataBufferHolder->resize(sampleDataSize);
					m_dataBuffer = &(*m_dataBufferHolder)[0];

					SampleNumberType copiedSample = 0;
					SampleNumberType writtenSampleCountBuffer;
					size_t readOffset = 0;
					size_t readBytes;
					while ((readBytes = soundProvider->FillSampleData(m_dataBuffer + readOffset, sampleDataSize - readOffset, copiedSample, &writtenSampleCountBuffer)) > 0)
					{
						copiedSample += writtenSampleCountBuffer;
						readOffset += readBytes;
						if (readOffset >= sampleDataSize)
						{
							break;
						}
					}

					if (copiedSample != m_bufferSampleSize)
					{
						m_bufferSampleSize = static_cast<SampleNumberType>(copiedSample);
					}
				}
			}

			//------------------------------------------------------------------------------
			void OboeBufferedSoundPlayingContext::FillStreamData(void *dataBuffer, void *targetBuffer, int frameCount, bool isApplyVolume, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount)
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
			bool OboeBufferedSoundPlayingContext::RenderAudio(void *audioData, int32_t numFrames, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount)
			{
				if (m_isPlayFinished || m_isPaused || m_isAllDataStreamed)
				{
					return false;
				}

				int32_t remainFrameNumber = numFrames;
				uint8_t *currentTargetBufferHead = (uint8_t *)audioData;
				while (remainFrameNumber > 0)
				{
					// Lock Current
					bool isCurLocked = false;
					while (!m_currentReadHeaderLock.compare_exchange_strong(isCurLocked, true))
					{
						// Hot Waiting to non-blocking
					}
					auto releaseCurrentLockFunc = [this]() mutable { m_currentReadHeaderLock.store(false); };
					FINALLY_OF_BLOCK(_RELEASE_CUR_LOCK, releaseCurrentLockFunc);

					// Fill with cached buffer
					if (m_bufferSampleSize > 0)
					{
						SampleNumberType writingSamples = m_bufferSampleSize - m_currentReadPosition;
						if (writingSamples <= 0)
						{
							if (m_isLooping)
							{
								m_currentReadPosition = 0;
								continue;
							}
							else
							{
								// fill 0 for remain buffer
								if (streamDataFormat == oboe::AudioFormat::Float)
								{
									size_t remainZeroingValues = static_cast<size_t>(remainFrameNumber * streamChannelCount);
									float *typedZeroingAudioDataBuffer = (float *)currentTargetBufferHead;
									for (size_t i = 0; i < remainZeroingValues; ++i)
									{
										typedZeroingAudioDataBuffer[i] = 0.0f;
									}
								}
								else
								{
									memset(currentTargetBufferHead, 0, remainFrameNumber * streamDataFrameBytes);
								}
								remainFrameNumber = 0;
								m_isAllDataStreamed = true;
								break;
							}
						}
						if (writingSamples > remainFrameNumber)
						{
							writingSamples = remainFrameNumber;
						}

						FillStreamData(m_dataBuffer + static_cast<size_t>(m_currentReadPosition) * m_soundSampleByteSize, currentTargetBufferHead, writingSamples, true, streamDataFrameBytes, streamDataFormat, streamChannelCount);

						m_currentReadPosition += writingSamples;
						remainFrameNumber -= writingSamples;
						currentTargetBufferHead += (writingSamples * streamDataFrameBytes);
					}
					else
					{
						// When no buffer, fill 0
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
						m_isAllDataStreamed = true;
					}
				}

				return true;
			}
		}
	}
}
