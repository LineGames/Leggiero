////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeStream.h (Leggiero/Modules - Sound)
//
// Oboe Stream Holder Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeStream.h"

// Standard Library
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Sound
#include "../Provider/ISoundProvider.h"
#include "OboeSoundMixer.h"
#include "_OboeInternal.h"
#include "IOboeStreamRendering.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			//////////////////////////////////////////////////////////////////////////////// Code-Level Settings

			namespace _Internal
			{
				constexpr size_t kMixingBufferSampleSize = 8192;
				constexpr int kStreamCreationErrorRetryCount = 3;
			}


			//////////////////////////////////////////////////////////////////////////////// OboeStream

			//------------------------------------------------------------------------------
			OboeStream::OboeStream(SamplingFrequencyType samplingRate)
				: m_requestedSamplingRate(static_cast<int32_t>(samplingRate))
				, m_streamCreationError(0), m_isSupposedToStart(false)
				, m_isBufferDirty(false)
				, m_oboeRenderingBuffer(new _Internal::RenderingQueueBuffer()), m_oboeReadyBuffer(new _Internal::RenderingQueueBuffer()), m_renderingTargetBuffer(new _Internal::RenderingQueueBuffer())
				, m_currentQueueVersion(1)
				, m_mixingTempBuffer(nullptr), m_hdrMixingBuffer(nullptr), m_currentMixingBufferFormat(oboe::AudioFormat::Invalid)
			{
				_CreateStream(true);
			}

			//------------------------------------------------------------------------------
			OboeStream::~OboeStream()
			{
				if (m_stream)
				{
					m_stream->close();
				}

				if (m_mixingTempBuffer != nullptr)
				{
					free(m_mixingTempBuffer);
				}
				if (m_hdrMixingBuffer != nullptr)
				{
					free(m_hdrMixingBuffer);
				}

				if (m_oboeRenderingBuffer != nullptr)
				{
					delete m_oboeRenderingBuffer;
				}

				_Internal::RenderingQueueBuffer *bufferCopy = m_oboeReadyBuffer.load();
				if (bufferCopy != nullptr)
				{
					delete bufferCopy;
				}

				if (m_renderingTargetBuffer != nullptr)
				{
					delete m_renderingTargetBuffer;
				}
			}

			//------------------------------------------------------------------------------
			void OboeStream::StartStream()
			{
				m_isSupposedToStart = true;
				std::shared_ptr<oboe::AudioStream> stream(m_stream);
				if (stream)
				{
					stream->requestStart();
				}
			}

			//------------------------------------------------------------------------------
			void OboeStream::StopStream()
			{
				m_isSupposedToStart = false;
				std::shared_ptr<oboe::AudioStream> stream(m_stream);
				if (stream)
				{
					stream->requestStop();
				}
			}

			//------------------------------------------------------------------------------
			void OboeStream::_CreateStream(bool isForceTry)
			{
				if (!isForceTry && m_streamCreationError > _Internal::kStreamCreationErrorRetryCount)
				{
					return;
				}

				oboe::AudioStreamBuilder builder;
				builder.setSharingMode(oboe::SharingMode::Shared)
					->setPerformanceMode(oboe::PerformanceMode::LowLatency)
					->setUsage(oboe::Usage::Game)
					->setChannelCount(2)
					->setSampleRate(m_requestedSamplingRate)
					->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
					->setFormat(oboe::AudioFormat::I16)
					->setDataCallback(this)
					->setErrorCallback(this);
				oboe::Result res = builder.openStream(m_stream);
				if (res != oboe::Result::OK)
				{
					// Stream Creation Error
					++m_streamCreationError;
					m_stream.reset();
					return;
				}

				// Fill Metadata
				m_streamChannelCount = m_stream->getChannelCount();
				m_streamDataFrameBytes = m_stream->getBytesPerFrame();
				m_streamDataFormat = m_stream->getFormat();
				if (m_streamChannelCount == 0 || (m_streamDataFormat != oboe::AudioFormat::I16 && m_streamDataFormat != oboe::AudioFormat::Float))
				{
					// Invalid Setting
					++m_streamCreationError;
					m_stream.reset();
					return;
				}

				if (m_streamDataFormat != m_currentMixingBufferFormat)
				{
					if (m_mixingTempBuffer != nullptr)
					{
						free(m_mixingTempBuffer);
						m_mixingTempBuffer = nullptr;
					}
					if (m_hdrMixingBuffer != nullptr)
					{
						free(m_hdrMixingBuffer);
						m_hdrMixingBuffer = nullptr;
					}

					size_t sampleSize = 0;
					size_t hdrSampleSize = 0;
					switch (m_streamDataFormat)
					{
						case oboe::AudioFormat::I16:
							sampleSize = sizeof(int16_t);
							hdrSampleSize = sizeof(int32_t);
							break;

						case oboe::AudioFormat::Float:
							sampleSize = sizeof(float);
							hdrSampleSize = sizeof(float);
							break;
					}

					if (sampleSize > 0)
					{
						m_mixingTempBuffer = malloc(_Internal::kMixingBufferSampleSize * sampleSize);
						m_hdrMixingBuffer = malloc(_Internal::kMixingBufferSampleSize * hdrSampleSize);
					}
					m_currentMixingBufferFormat = m_streamDataFormat;
				}

				m_streamCreationError = 0;
			}

			//------------------------------------------------------------------------------
			oboe::DataCallbackResult OboeStream::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames)
			{
				_Internal::RenderingQueueBuffer *readyBuffer = m_oboeReadyBuffer.load();
				if (readyBuffer->queueVersion.load() > m_oboeRenderingBuffer->queueVersion.load())
				{
					_Internal::RenderingQueueBuffer *bufferCopyMemory = m_oboeRenderingBuffer;
					m_oboeRenderingBuffer = m_oboeReadyBuffer.exchange(bufferCopyMemory);
				}

				std::vector<std::shared_ptr<IOboeStreamRendering> > &renderingQueue = m_oboeRenderingBuffer->renderingQueue;

				if (renderingQueue.empty())
				{
					if (m_streamDataFormat == oboe::AudioFormat::Float)
					{
						size_t fillingValues = static_cast<size_t>(numFrames * m_streamChannelCount);
						float *typedAudioDataBuffer = (float *)audioData;
						for (size_t i = 0; i < fillingValues; ++i)
						{
							typedAudioDataBuffer[i] = 0.0f;
						}
					}
					else
					{
						memset(audioData, 0, numFrames * m_streamDataFrameBytes);
					}
					return oboe::DataCallbackResult::Continue;
				}

				// Do Rendering by Type
				size_t valueCount = static_cast<size_t>(numFrames * m_streamChannelCount);
				switch (m_streamDataFormat)
				{
					case oboe::AudioFormat::I16:
						{
							memset(m_hdrMixingBuffer, 0, valueCount * sizeof(int32_t));
							int32_t *hdrMixingBuffer = (int32_t *)m_hdrMixingBuffer;
							int16_t *mixingTempBuffer = (int16_t *)m_mixingTempBuffer;
							for (std::shared_ptr<IOboeStreamRendering> &currentRenderable : renderingQueue)
							{
								if (currentRenderable->RenderAudio(m_mixingTempBuffer, numFrames, m_streamDataFrameBytes, m_streamDataFormat, m_streamChannelCount))
								{
									for (size_t i = 0; i < valueCount; ++i)
									{
										hdrMixingBuffer[i] += mixingTempBuffer[i];
									}
								}
							}

							int16_t *typedAudioDataBuffer = (int16_t *)audioData;
							for (size_t i = 0; i < valueCount; ++i)
							{
								typedAudioDataBuffer[i] = static_cast<int16_t>(Utility::Math::ClipMinMax<int32_t>(hdrMixingBuffer[i], std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()));
							}
						}
						break;

					case oboe::AudioFormat::Float:
						{
							float *hdrMixingBuffer = (float *)m_hdrMixingBuffer;
							for (size_t i = 0; i < valueCount; ++i)
							{
								hdrMixingBuffer[i] = 0.0f;
							}
							float *mixingTempBuffer = (float *)m_mixingTempBuffer;
							for (std::shared_ptr<IOboeStreamRendering> &currentRenderable : renderingQueue)
							{
								if (currentRenderable->RenderAudio(m_mixingTempBuffer, numFrames, m_streamDataFrameBytes, m_streamDataFormat, m_streamChannelCount))
								{
									for (size_t i = 0; i < valueCount; ++i)
									{
										hdrMixingBuffer[i] += mixingTempBuffer[i];
									}
								}
							}

							float *typedAudioDataBuffer = (float *)audioData;
							for (size_t i = 0; i < valueCount; ++i)
							{
								typedAudioDataBuffer[i] = Utility::Math::ClipMinMax<float>(hdrMixingBuffer[i], -1.0f, 1.0f);
							}
						}
						break;
				}
				return oboe::DataCallbackResult::Continue;
			}

			//------------------------------------------------------------------------------
			void OboeStream::onErrorAfterClose(oboe::AudioStream *audioStream, oboe::Result error)
			{
				_CreateStream(false);
				if (m_stream && m_isSupposedToStart)
				{
					m_stream->requestStart();
				}
			}

			//------------------------------------------------------------------------------
			void OboeStream::_RegisterRendering(std::shared_ptr<IOboeStreamRendering> rendering)
			{
				if (!rendering)
				{
					return;
				}

				// Do in Lock
				auto lockContext = m_bufferRenderingLock.Lock();
				if (lockContext)
				{
					IOboeStreamRendering *ptrKey = rendering.get();
					if (m_streamRenderings.find(ptrKey) != m_streamRenderings.end())
					{
						// Duplicated Entry, but can be occurred
						return;
					}
					m_streamRenderings.insert(std::make_pair(ptrKey, rendering));

					// Below is simplified version of _MakeRenderQueue, but copied here for same lock
					for (std::unordered_map<IOboeStreamRendering *, std::shared_ptr<IOboeStreamRendering> >::iterator it = m_streamRenderings.begin(); it != m_streamRenderings.end(); ++it)
					{
						m_renderingTargetBuffer->renderingQueue.push_back(it->second);
					}

					m_renderingTargetBuffer->queueVersion.store(m_currentQueueVersion.fetch_add(1));

					_Internal::RenderingQueueBuffer *bufferCopyMemory = m_renderingTargetBuffer;
					m_renderingTargetBuffer = m_oboeReadyBuffer.exchange(bufferCopyMemory);
					m_renderingTargetBuffer->renderingQueue.clear();

					return;
				}
				else
				{
					// If cannot aquire lock, the sound can be missed
				}
			}

			//------------------------------------------------------------------------------
			bool OboeStream::_MakeRenderQueue()
			{
				// Do in Lock
				auto lockContext = m_bufferRenderingLock.Lock();
				if (lockContext)
				{
					// Build Queue into m_renderingTargetBuffer and Trimming
					for (std::unordered_map<IOboeStreamRendering *, std::shared_ptr<IOboeStreamRendering> >::iterator it = m_streamRenderings.begin(); it != m_streamRenderings.end(); ++it)
					{
						if (it->first->IsStoppedRendering())
						{
							m_renderingWorkingBuffer1.push_back(it->first);
						}
						else if (it->first->IsPausedRendering() && !it->first->IsHighPriorityObject())
						{
							m_renderingWorkingBuffer2.push_back(it->first);
						}
						else
						{
							m_renderingTargetBuffer->renderingQueue.push_back(it->second);
						}
					}
					for (IOboeStreamRendering *currentRemoving : m_renderingWorkingBuffer1)
					{
						m_streamRenderings.erase(currentRemoving);
					}
					m_renderingWorkingBuffer1.clear();

					for (std::list<std::weak_ptr<IOboeStreamRendering> >::iterator it = m_weakHoldingRenderings.begin(); it != m_weakHoldingRenderings.end(); )
					{
						std::shared_ptr<IOboeStreamRendering> sharedRendering(it->lock());
						if (sharedRendering)
						{
							IOboeStreamRendering *renderingRawPointer = sharedRendering.get();
							if (renderingRawPointer->IsStoppedRendering())
							{
								it = m_weakHoldingRenderings.erase(it);
							}
							else if (!renderingRawPointer->IsPausedRendering())
							{
								// Re-played
								if (m_streamRenderings.find(renderingRawPointer) == m_streamRenderings.end())
								{
									m_streamRenderings.insert(std::make_pair(renderingRawPointer, sharedRendering));
									m_renderingTargetBuffer->renderingQueue.push_back(sharedRendering);
								}
								it = m_weakHoldingRenderings.erase(it);
							}
							else
							{
								++it;
							}
						}
						else
						{
							it = m_weakHoldingRenderings.erase(it);
						}
					}

					for (IOboeStreamRendering *currentWeakering : m_renderingWorkingBuffer2)
					{
						m_weakHoldingRenderings.push_back(m_streamRenderings[currentWeakering]);
						m_streamRenderings.erase(currentWeakering);
					}
					m_renderingWorkingBuffer2.clear();

					m_renderingTargetBuffer->queueVersion.store(m_currentQueueVersion.fetch_add(1));

					_Internal::RenderingQueueBuffer *bufferCopyMemory = m_renderingTargetBuffer;
					m_renderingTargetBuffer = m_oboeReadyBuffer.exchange(bufferCopyMemory);
					m_renderingTargetBuffer->renderingQueue.clear();

					return true;
				}
				
				// Something Wrong, but make back up
				m_isBufferDirty.store(true);
				return false;
			}

			//------------------------------------------------------------------------------
			void OboeStream::_MakeQueueIfNeeded()
			{
				bool isDirty = true;
				if (m_isBufferDirty.compare_exchange_strong(isDirty, false))
				{
					_MakeRenderQueue();
				}
			}
		}
	}
}
