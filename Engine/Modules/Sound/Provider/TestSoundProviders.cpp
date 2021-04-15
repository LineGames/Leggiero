////////////////////////////////////////////////////////////////////////////////
// Provider/TestSoundProviders.h (Leggiero/Modules - Sound)
//
// Test sound providers implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TestSoundProviders.h"

// Standard Library
#include <cmath>
#include <cstdint>
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Math/SimpleGeometry.h>


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// SineWaveSoundProvider

		//------------------------------------------------------------------------------
		SineWaveSoundProvider::SineWaveSoundProvider(SampleFormat format, SamplingFrequencyType sampleFrequency, float length, float waveFrequency, float amplitude)
			: m_format(format), m_frequency(sampleFrequency), m_lengthInSecond(length), m_waveFrequency(waveFrequency), m_waveAmplitude(amplitude)
			, m_bakedWave(nullptr), m_bakedPeriodLength(0)
		{
			if (!std::isfinite(m_waveAmplitude))
			{
				m_waveAmplitude = 0.0f;
			}
			_BakePreSampledWave();
		}

		//------------------------------------------------------------------------------
		SineWaveSoundProvider::~SineWaveSoundProvider()
		{
			free(m_bakedWave);
		}

		//------------------------------------------------------------------------------
		size_t SineWaveSoundProvider::FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples)
		{
			size_t sampleSize = GetSampleByteSize(m_format);
			SampleNumberType sampleCount = static_cast<SampleNumberType>(bufferSize / sampleSize);

			SampleNumberType endSample = startSample + sampleCount;
			size_t maxSampleCount = GetSampleLength();
			if (endSample > maxSampleCount)
			{
				endSample = maxSampleCount;
			}

			SampleNumberType writtenSampleCount = 0;

			if (m_bakedWave == nullptr)
			{
				// Realtime Generation
				float effectiveFrequency = 0.0f;
				if (std::isfinite(m_waveFrequency))
				{
					effectiveFrequency = m_waveFrequency;
				}
				for (SampleNumberType i = startSample; i < endSample; ++i)
				{
					float currentTime = (float)i / (float)m_frequency;
					float currentValue = static_cast<float>(sin(currentTime * effectiveFrequency * Utility::Mathf::kPi) * m_waveAmplitude);
					if (currentValue > 1.0f)
					{
						currentValue = 1.0f;
					}
					else if (currentValue < -1.0f)
					{
						currentValue = -1.0f;
					}
					switch (m_format)
					{
						case SampleFormat::kMono_8:
							{
								uint8_t pcmValue = static_cast<uint8_t>(currentValue * 127.0f + 128.0f);
								*((uint8_t *)dataBuffer + writtenSampleCount * sampleSize) = pcmValue;
							}
							break;

						case SampleFormat::kMono_16:
							{
								int16_t pcmValue = static_cast<int16_t>(currentValue * 32767.0f);
								*(int16_t *)((uint8_t *)dataBuffer + writtenSampleCount * sampleSize) = pcmValue;
							}
							break;

						case SampleFormat::kStereo_8:
							{
								uint8_t pcmValue = static_cast<uint8_t>(currentValue * 127.0f + 128.0f);
								*((uint8_t *)dataBuffer + writtenSampleCount * sampleSize) = pcmValue;
								*((uint8_t *)dataBuffer + writtenSampleCount * sampleSize + 1) = pcmValue;
							}
							break;

						case SampleFormat::kStereo_16:
							{
								int16_t pcmValue = static_cast<int16_t>(currentValue * 32767.0f);
								*(int16_t *)((uint8_t *)dataBuffer + writtenSampleCount * sampleSize) = pcmValue;
								*(int16_t *)((uint8_t *)dataBuffer + writtenSampleCount * sampleSize + 2) = pcmValue;
							}
							break;
					}
					++writtenSampleCount;
				}
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = writtenSampleCount;
				}
			}
			else
			{
				for (SampleNumberType i = startSample; i < endSample;)
				{
					SampleNumberType bakedOffset = i % m_bakedPeriodLength;
					SampleNumberType copySampleLength = m_bakedPeriodLength - bakedOffset;
					if (i + copySampleLength > endSample)
					{
						copySampleLength = endSample - i;
					}
					if (copySampleLength > 0)
					{
						memcpy((uint8_t *)dataBuffer + static_cast<size_t>(writtenSampleCount) * sampleSize, (uint8_t *)m_bakedWave + bakedOffset * sampleSize, static_cast<size_t>(copySampleLength) * sampleSize);
						writtenSampleCount += copySampleLength;
						i += copySampleLength;
					}
				}
				if (outWrittenSamples != nullptr)
				{
					*outWrittenSamples = writtenSampleCount;
				}
			}

			// Prevent Glitch
			if (endSample == maxSampleCount)
			{
				int decayWindowSize = 16;
				if (decayWindowSize > writtenSampleCount)
				{
					decayWindowSize = (int)writtenSampleCount;
				}
				if (decayWindowSize > 1)
				{
					float decayMultiplier = 1.0f / static_cast<float>(decayWindowSize);
					float currentMultiplier = 1.0f;
					SampleNumberType decayStart = endSample - decayWindowSize;
					SampleNumberType bufferOffsetSampleCount = writtenSampleCount - decayWindowSize;
					for (SampleNumberType i = decayStart; i < endSample; ++i)
					{
						switch (m_format)
						{
							case SampleFormat::kMono_8:
								{
									*((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize) = static_cast<uint8_t>(static_cast<float>(*((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize)) * currentMultiplier);
								}
								break;

							case SampleFormat::kMono_16:
								{
									*(int16_t *)((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize) = static_cast<int16_t>(static_cast<float>(*(short *)((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize)) * currentMultiplier);
								}
								break;

							case SampleFormat::kStereo_8:
								{
									*((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize) = static_cast<uint8_t>(static_cast<float>(*((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize)) * currentMultiplier);
									*((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize + 1) = static_cast<uint8_t>(static_cast<float>(*((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize + 1)) * currentMultiplier);
								}
								break;

							case SampleFormat::kStereo_16:
								{
									*(int16_t *)((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize) = static_cast<int16_t>(static_cast<float>(*(int16_t *)((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize)) * currentMultiplier);
									*(int16_t *)((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize + 2) = static_cast<int16_t>(static_cast<float>(*(int16_t *)((uint8_t *)dataBuffer + bufferOffsetSampleCount * sampleSize + 2)) * currentMultiplier);
								}
								break;
						}
						currentMultiplier -= decayMultiplier;
						++bufferOffsetSampleCount;
					}
				}
			}

			return static_cast<size_t>(writtenSampleCount * sampleSize);
		}

		//------------------------------------------------------------------------------
		void SineWaveSoundProvider::_BakePreSampledWave()
		{
			if (!std::isfinite(m_waveFrequency))
			{
				// invalid Frequency
				return;
			}
			if (fabs(m_waveFrequency) < 2.0f)
			{
				// Too Slow to Pre-Bake
				return;
			}
			if (fabs(m_waveFrequency) * 2.0f >= m_frequency)
			{
				// To Fast to Sample (Nyquist)
				return;
			}

			// Prevent TOO SHORT baked period
			float periodMultiplier = 1.0f;
			m_bakedPeriodLength = 0;
			while (m_bakedPeriodLength < 256)
			{
				m_bakedPeriodLength = (size_t)(periodMultiplier * m_frequency / fabs(m_waveFrequency));
				periodMultiplier *= 2.0f;
			}

			size_t sampleSize = GetSampleByteSize(m_format);
			m_bakedWave = malloc(m_bakedPeriodLength * sampleSize);
			for (size_t i = 0; i < m_bakedPeriodLength; ++i)
			{
				float currentValue = float(sin(((float)i / (float)m_frequency) * m_waveFrequency * Utility::Mathf::kPi) * m_waveAmplitude);
				if (currentValue > 1.0f)
				{
					currentValue = 1.0f;
				}
				else if (currentValue < -1.0f)
				{
					currentValue = -1.0f;
				}
				switch (m_format)
				{
					case SampleFormat::kMono_8:
						{
							uint8_t pcmValue = static_cast<uint8_t>(currentValue * 127.0f + 128.0f);
							*((uint8_t *)m_bakedWave + i * sampleSize) = pcmValue;
						}
						break;

					case SampleFormat::kMono_16:
						{
							int16_t pcmValue = static_cast<int16_t>(currentValue * 32767.0f);
							*(int16_t *)((uint8_t *)m_bakedWave + i * sampleSize) = pcmValue;
						}
						break;

					case SampleFormat::kStereo_8:
						{
							uint8_t pcmValue = static_cast<uint8_t>(currentValue * 127.0f + 128.0f);
							*((uint8_t *)m_bakedWave + i * sampleSize) = pcmValue;
							*((uint8_t *)m_bakedWave + i * sampleSize + 1) = pcmValue;
						}
						break;

					case SampleFormat::kStereo_16:
						{
							int16_t pcmValue = static_cast<int16_t>(currentValue * 32767.0f);
							*(int16_t *)((uint8_t *)m_bakedWave + i * sampleSize) = pcmValue;
							*(int16_t *)((uint8_t *)m_bakedWave + i * sampleSize + 2) = pcmValue;
						}
						break;
				}
			}
		}
	}
}
