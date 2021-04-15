////////////////////////////////////////////////////////////////////////////////
// Provider/LoopHoldSoundProvider.cpp (Leggiero/Modules - Sound)
//
// Sound Provider Looper Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "LoopHoldSoundProvider.h"

// Standard Library
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// LoopHoldSoundProvider

		//------------------------------------------------------------------------------
		LoopHoldSoundProvider::LoopHoldSoundProvider(ISoundProvider *pSource, SampleNumberType loopStartSample, SampleNumberType loopFinishSample, bool isStartLooping)
			: m_format(pSource->GetFormat()), m_frequency(pSource->GetFrequency())
			, m_pSource(pSource), m_sharedSourceReference(nullptr)
			, m_isInitialized(false)
			, m_isLooping(isStartLooping), m_loopStartSample(loopStartSample), m_loopFinishSample(loopFinishSample)
		{
			m_isInitialized = true;
		}

		//------------------------------------------------------------------------------
		LoopHoldSoundProvider::LoopHoldSoundProvider(std::shared_ptr<ISoundProvider> sharedSource, SampleNumberType loopStartSample, SampleNumberType loopFinishSample, bool isStartLooping)
			: m_format(sharedSource->GetFormat()), m_frequency(sharedSource->GetFrequency())
			, m_pSource(nullptr), m_sharedSourceReference(sharedSource)
			, m_isInitialized(false)
			, m_isLooping(isStartLooping), m_loopStartSample(loopStartSample), m_loopFinishSample(loopFinishSample)
		{
			m_isInitialized = true;
		}

		//------------------------------------------------------------------------------
		LoopHoldSoundProvider::~LoopHoldSoundProvider()
		{
			if (m_isInitialized)
			{
				m_isInitialized = false;
			}
		}

		//------------------------------------------------------------------------------
		size_t LoopHoldSoundProvider::FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
		{
			if (!m_isInitialized)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			if (m_pSource != nullptr)
			{
				return _FillSampleData(m_pSource, dataBuffer, bufferSize, startSample, outWrittenSamples);
			}
			else if (m_sharedSourceReference)
			{
				return _FillSampleData(m_sharedSourceReference.get(), dataBuffer, bufferSize, startSample, outWrittenSamples);
			}

			// No Source
			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = 0;
			}
			return 0;
		}

		//------------------------------------------------------------------------------
		size_t LoopHoldSoundProvider::_FillSampleData(ISoundProvider *pSource, void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
		{
			if (!m_isLooping)
			{
				return pSource->FillSampleData(dataBuffer, bufferSize, startSample, outWrittenSamples);
			}
			if (m_loopStartSample >= m_loopFinishSample)
			{
				// Invalid State
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			size_t sampleSize = GetSampleByteSize(m_format);
			SampleNumberType sampleCount = static_cast<SampleNumberType>(bufferSize / sampleSize);
			if (sampleCount == 0)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			SampleNumberType endSample = startSample + sampleCount;
			if (endSample <= m_loopFinishSample)
			{
				// Not Need to Loop
				return pSource->FillSampleData(dataBuffer, bufferSize, startSample, outWrittenSamples);
			}

			SampleNumberType sampleCountOutBuffer;

			SampleNumberType writtenSample = 0;
			size_t writtenBytes = 0;
			if (startSample < m_loopStartSample)
			{
				SampleNumberType remainPreReadSample = m_loopStartSample - startSample;
				while (remainPreReadSample > 0)
				{
					size_t preReadBufferSize = (size_t)(remainPreReadSample * sampleSize);
					if (preReadBufferSize > bufferSize - writtenBytes)
					{
						preReadBufferSize = bufferSize - writtenBytes;
					}
					size_t currentReadBytes = pSource->FillSampleData((void *)((uint8_t *)dataBuffer + writtenBytes), preReadBufferSize, startSample + writtenSample, &sampleCountOutBuffer);
					if (currentReadBytes == 0)
					{
						// Source Finished before Looping Section
						if (outWrittenSamples != nullptr)
						{
							*outWrittenSamples = writtenSample;
						}
						return writtenBytes;
					}

					writtenSample += sampleCountOutBuffer;
					writtenBytes += currentReadBytes;
					remainPreReadSample -= sampleCountOutBuffer;
				}
			}

			SampleNumberType currentReadStart = startSample + writtenSample;
			if (currentReadStart > m_loopFinishSample)
			{
				currentReadStart = m_loopStartSample + ((currentReadStart - m_loopStartSample) % (m_loopFinishSample - m_loopStartSample));
			}

			SampleNumberType currentReadFinish = currentReadStart + (sampleCount - writtenSample);

			SampleNumberType effectiveLoopFinish = m_loopFinishSample;

			// Read Front Cutted Part
			if (currentReadStart > m_loopStartSample && currentReadFinish >= effectiveLoopFinish)
			{
				SampleNumberType remainFrontPartSample = effectiveLoopFinish - currentReadStart;
				while (remainFrontPartSample > 0)
				{
					size_t frontPartBufferSize = static_cast<size_t>(remainFrontPartSample * sampleSize);
					if (frontPartBufferSize > bufferSize - writtenBytes)
					{
						frontPartBufferSize = bufferSize - writtenBytes;
					}
					size_t currentReadBytes = pSource->FillSampleData((void *)((uint8_t *)dataBuffer + writtenBytes), frontPartBufferSize, effectiveLoopFinish - remainFrontPartSample, &sampleCountOutBuffer);
					if (currentReadBytes == 0)
					{
						// Source Finished Before Loop Finish
						effectiveLoopFinish = effectiveLoopFinish - remainFrontPartSample;
						break;
					}

					writtenSample += sampleCountOutBuffer;
					writtenBytes += currentReadBytes;
					remainFrontPartSample -= sampleCountOutBuffer;
				}
				currentReadStart = m_loopStartSample;
				currentReadFinish = currentReadStart + (sampleCount - writtenSample);
			}
			if (effectiveLoopFinish <= m_loopStartSample)
			{
				// Invalid Loop Range
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = writtenSample;
				}
				return writtenBytes;
			}

			// Read Looping Part
			if (currentReadFinish >= effectiveLoopFinish)
			{
				SampleNumberType loopSampleSize = effectiveLoopFinish - currentReadStart;
				size_t loopBufferSize = (size_t)(loopSampleSize * sampleSize);
				unsigned char *loopBuffer = (unsigned char *)malloc(loopBufferSize);
				if (loopBuffer == nullptr)
				{
					// Out of Memory
					if (outWrittenSamples != nullptr)
					{
						*outWrittenSamples = writtenSample;
					}
					return writtenBytes;
				}

				auto bufferReleaseFunc = [loopBuffer] { free(loopBuffer); };
				FINALLY_OF_BLOCK(_LOOP_BUFFER_RELEASE, bufferReleaseFunc);

				size_t currentReadBytes = pSource->FillSampleData((void *)loopBuffer, loopBufferSize, currentReadStart, &sampleCountOutBuffer);
				if (currentReadBytes == 0)
				{
					// Invalid Loop Range
					if (outWrittenSamples != nullptr)
					{
						*outWrittenSamples = writtenSample;
					}
					return writtenBytes;
				}

				SampleNumberType loopSampleEffectiveSize = loopSampleSize;
				size_t loopBufferEffectiveSize = loopBufferSize;
				if (sampleCountOutBuffer != (effectiveLoopFinish - currentReadStart))
				{
					effectiveLoopFinish = currentReadStart + sampleCountOutBuffer;
					loopSampleEffectiveSize = sampleCountOutBuffer;
					loopBufferEffectiveSize = (size_t)(sampleCountOutBuffer * sampleSize);
				}

				// Cached Copy
				while (currentReadFinish >= effectiveLoopFinish)
				{
					memcpy((void *)((uint8_t *)dataBuffer + writtenBytes), loopBuffer, loopBufferEffectiveSize);
					writtenSample += loopSampleEffectiveSize;
					writtenBytes += loopBufferEffectiveSize;
					currentReadFinish -= loopSampleEffectiveSize;
				}

				// Process Remain Part using Cache
				if (currentReadFinish > currentReadStart)
				{
					SampleNumberType remainPartSample = currentReadFinish - currentReadStart;
					size_t remainPartSize = static_cast<size_t>(remainPartSample * sampleSize);
					memcpy((void *)((uint8_t *)dataBuffer + writtenBytes), loopBuffer, remainPartSize);
					writtenSample += remainPartSample;
					writtenBytes += remainPartSize;
					currentReadFinish -= remainPartSample;
				}
			}

			// Read Back Remain Part
			while (currentReadStart < currentReadFinish)
			{
				size_t backPartBufferSize = bufferSize - writtenBytes;
				size_t currentReadBytes = pSource->FillSampleData((void *)((uint8_t *)dataBuffer + writtenBytes), backPartBufferSize, currentReadStart, &sampleCountOutBuffer);
				if (currentReadBytes == 0)
				{
					if (currentReadStart > m_loopStartSample)
					{
						// Read from Loop Start
						currentReadStart = m_loopStartSample;
						currentReadFinish = currentReadStart + (sampleCount - writtenSample);
						continue;
					}
					else
					{
						// Invalid Loop Range
						if (outWrittenSamples != nullptr)
						{
							*outWrittenSamples = writtenSample;
						}
						return writtenBytes;
					}
				}

				writtenSample += sampleCountOutBuffer;
				writtenBytes += currentReadBytes;
				currentReadStart += sampleCountOutBuffer;
				currentReadFinish = currentReadStart + (sampleCount - writtenSample);
			}

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = writtenSample;
			}
			return writtenBytes;
		}
	}
}
