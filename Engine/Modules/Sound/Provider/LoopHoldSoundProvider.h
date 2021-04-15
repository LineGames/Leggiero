////////////////////////////////////////////////////////////////////////////////
// Provider/LoopHoldSoundProvider.h (Leggiero/Modules - Sound)
//
// Sound Provider Looper
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__LOOP_HOLD_SOUND_PROVIDER_H
#define __LM_SOUND__PROVIDER__LOOP_HOLD_SOUND_PROVIDER_H


// Standard Library
#include <memory>

// Leggiero.Sound
#include "ISoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		// Sound data provider to loop given interval
		// THIS PROVIDER is NOT SEEKABLE. This means, you can not use this provider for ordinary case.
		class LoopHoldSoundProvider
			: public ISoundProvider
		{
		public:
			LoopHoldSoundProvider(ISoundProvider *pSource, SampleNumberType loopStartSample = 0, SampleNumberType loopFinishSample = 0, bool isStartLooping = false);
			LoopHoldSoundProvider(std::shared_ptr<ISoundProvider> sharedSource, SampleNumberType loopStartSample = 0, SampleNumberType loopFinishSample = 0, bool isStartLooping = false);
			virtual ~LoopHoldSoundProvider();

		protected:
			// Prevent Creating of NULL Sound Provider
			LoopHoldSoundProvider() { }

		public:	// ISoundProvider
			virtual SampleFormat			GetFormat() const override { return m_format; }
			virtual SamplingFrequencyType	GetFrequency() const override { return m_frequency; }

			// Length is Meaningless for LoopHoldSoundProvider
			virtual size_t	GetSampleLength() const override { return 0; }
			virtual float	GetLengthInSecond() const override { return 0.0f; }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) override;

		public:
			bool IsLooping() const { return m_isLooping; }
			void SetLooping(bool isLooping) { m_isLooping = isLooping; }

			SampleNumberType GetLoopStartSample() const { return m_loopStartSample; }
			SampleNumberType GetLoopFinishSample() const { return m_loopFinishSample; }
			void SetLoopInterval(SampleNumberType start, SampleNumberType finish)
			{
				m_loopStartSample = start;
				m_loopFinishSample = finish;
			}

		protected:
			size_t	_FillSampleData(ISoundProvider *pSource, void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples);

		public:
			bool IsValid() const { return m_isInitialized; }

		protected:
			bool	m_isInitialized;

			SampleFormat			m_format;
			SamplingFrequencyType	m_frequency;

			bool				m_isLooping;
			SampleNumberType	m_loopStartSample;
			SampleNumberType	m_loopFinishSample;

			ISoundProvider *m_pSource;
			std::shared_ptr<ISoundProvider> m_sharedSourceReference;
		};
	}
}

#endif
