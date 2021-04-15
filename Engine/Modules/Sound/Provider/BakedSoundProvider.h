////////////////////////////////////////////////////////////////////////////////
// Provider/BakedSoundProvider.h (Leggiero/Modules - Sound)
//
// Pre-baked sound provider
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__BAKED_SOUND_PROVIDER_H
#define __LM_SOUND__PROVIDER__BAKED_SOUND_PROVIDER_H


// Standard Library
#include <functional>
#include <memory>

// Leggiero.Sound
#include "ISoundProvider.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Utility
	{
		namespace Data
		{
			class MemoryBuffer;
		}
	}


	namespace Sound
	{
		// Pre-baked sound data provider using in-memory buffer
		class BakedSoundProvider
			: public ISoundProvider
		{
		public:
			BakedSoundProvider(ISoundProvider &source, std::shared_ptr<Utility::Data::MemoryBuffer> buffer = nullptr, 
				std::function<void(std::shared_ptr<Utility::Data::MemoryBuffer>)> bufferRelease = std::function<void(std::shared_ptr<Utility::Data::MemoryBuffer>)>());
			virtual ~BakedSoundProvider();

		private:
			// Prevent Creating of NULL Sound Provider
			BakedSoundProvider() = delete;

		public:	// ISoundProvider
			virtual SampleFormat			GetFormat() const override { return m_format; }
			virtual SamplingFrequencyType	GetFrequency() const override { return m_frequency; }

			virtual size_t	GetSampleLength() const override { return m_lengthInSample; }
			virtual float	GetLengthInSecond() const override { return m_lengthInSecond; }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) override;

		public:
			bool IsValid() const { return m_isInitialized; }

		protected:
			bool	m_isInitialized;

			SampleFormat			m_format;
			SamplingFrequencyType	m_frequency;

			size_t	m_lengthInSample;
			float	m_lengthInSecond;

			std::shared_ptr<Utility::Data::MemoryBuffer> m_buffer;
			std::function<void(std::shared_ptr<Utility::Data::MemoryBuffer>)> m_bufferRelease;
		};
	}
}

#endif
