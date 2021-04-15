////////////////////////////////////////////////////////////////////////////////
// Provider/BakedSoundProvider.cpp (Leggiero/Modules - Sound)
//
// Baked Sound Provider Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "BakedSoundProvider.h"

// Standard Library
#include <cstdlib>

// Leggiero.Utility
//#include <SyntacticSugar.h>
#include <Utility/Data/MemoryBuffer.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// BakedSoundProvider

		//------------------------------------------------------------------------------
		BakedSoundProvider::BakedSoundProvider(ISoundProvider &source, std::shared_ptr<Utility::Data::MemoryBuffer> buffer, std::function<void(std::shared_ptr<Utility::Data::MemoryBuffer>)> bufferRelease)
			: m_format(source.GetFormat()), m_frequency(source.GetFrequency()), m_lengthInSample(source.GetSampleLength()), m_lengthInSecond(source.GetLengthInSecond())
			, m_buffer(buffer), m_bufferRelease(bufferRelease)
			, m_isInitialized(false)
		{
			if (!m_buffer)
			{
				m_buffer = std::make_shared<Utility::Data::MemoryBuffer>();
			}

			size_t sampleSize = GetSampleByteSize(m_format);
			size_t sampleDataSize = static_cast<size_t>(sampleSize * m_lengthInSample);
			if (sampleDataSize == 0)
			{
				return;
			}

			if (m_buffer->GetCurrentSize() == 0)
			{
				// First Initialize
				m_buffer->ReserveBufferToSize(67108864); // 64MB
			}
			m_buffer->ReserveBufferToSize(sampleDataSize);

			SampleNumberType copiedSample = 0;
			SampleNumberType writtenSampleCountBuffer;
			size_t readOffset = 0;
			size_t readBytes;
			while ((readBytes = source.FillSampleData((void *)((uint8_t *)(m_buffer->GetBuffer()) + readOffset), sampleDataSize - readOffset, copiedSample, &writtenSampleCountBuffer)) > 0)
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

			if (copiedSample != m_lengthInSample)
			{
				m_lengthInSample = (size_t)copiedSample;
				m_lengthInSecond = (float)m_lengthInSample / (float)m_frequency;
			}

			m_isInitialized = true;
		}

		//------------------------------------------------------------------------------
		BakedSoundProvider::~BakedSoundProvider()
		{
			if (m_bufferRelease)
			{
				if (m_buffer)
				{
					m_bufferRelease(m_buffer);
				}
			}

			if (m_isInitialized)
			{
				m_isInitialized = false;
			}
		}

		//------------------------------------------------------------------------------
		size_t BakedSoundProvider::FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
		{
			if (!m_isInitialized)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			size_t sampleSize = GetSampleByteSize(GetFormat());
			SampleNumberType sampleCount = static_cast<SampleNumberType>(bufferSize / sampleSize);

			SampleNumberType endSample = startSample + sampleCount;
			size_t maxSampleCount = GetSampleLength();
			if (endSample > maxSampleCount)
			{
				endSample = maxSampleCount;
			}

			if (startSample >= endSample)
			{
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = 0;
				}
				return 0;
			}

			SampleNumberType writtenSampleCount = endSample - startSample;
			memcpy(dataBuffer, (uint8_t *)(m_buffer->GetBuffer()) + startSample * sampleSize, (size_t)writtenSampleCount * sampleSize);

			if (outWrittenSamples != nullptr)
			{
				*outWrittenSamples = writtenSampleCount;
			}
			return static_cast<size_t>(writtenSampleCount * sampleSize);
		}
	}
}
