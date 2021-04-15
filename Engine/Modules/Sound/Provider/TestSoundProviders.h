////////////////////////////////////////////////////////////////////////////////
// Provider/TestSoundProviders.h (Leggiero/Modules - Sound)
//
// Some example sound providers for testing purpose
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__TEST_SOUND_PROVIDERS_H
#define __LM_SOUND__PROVIDER__TEST_SOUND_PROVIDERS_H


// Leggiero.Sound
#include "ISoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		// Sine Wave Sound Data Provider
		class SineWaveSoundProvider
			: public ISoundProvider
		{
		public:
			SineWaveSoundProvider(SampleFormat format, SamplingFrequencyType sampleFrequency, float length, float waveFrequency, float amplitude = 1.0f);
			virtual ~SineWaveSoundProvider();

		protected:
			// Prevent Creating of NULL Sound Provider
			SineWaveSoundProvider() { }

		public:	// ISoundProvider
			virtual SampleFormat			GetFormat() const override { return m_format; }
			virtual SamplingFrequencyType	GetFrequency() const override { return m_frequency; }

			virtual size_t	GetSampleLength() const override { return (size_t)(m_lengthInSecond * (float)m_frequency); }
			virtual float	GetLengthInSecond() const override { return m_lengthInSecond; }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) override;

		protected:
			float m_lengthInSecond;
			float m_waveFrequency;
			float m_waveAmplitude;

			SampleFormat			m_format;
			SamplingFrequencyType	m_frequency;

			void	*m_bakedWave;
			size_t	m_bakedPeriodLength;

		protected:
			void _BakePreSampledWave();
		};
	}
}

#endif
