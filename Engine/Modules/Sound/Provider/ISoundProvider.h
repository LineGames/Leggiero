////////////////////////////////////////////////////////////////////////////////
// Provider/ISoundProvider.h (Leggiero/Modules - Sound)
//
// Common Interface for Sound Prodiver
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__I_SOUND_PROVIDER_H
#define __LM_SOUND__PROVIDER__I_SOUND_PROVIDER_H


// Standard Library
#include <cstring>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		// Sound Provider Interface
		class ISoundProvider
		{
		public:
			virtual ~ISoundProvider() { }

		protected:
			// Only can be created or copied by concrete sound providers
			ISoundProvider() { }
			ISoundProvider(const ISoundProvider &rhs) { }
			ISoundProvider &operator=(const ISoundProvider &rhs) { return *this; }

		public:
			virtual SampleFormat			GetFormat() const = 0;
			virtual SamplingFrequencyType	GetFrequency() const = 0;
			size_t							GetSingleSampleByteSize() const { return GetSampleByteSize(GetFormat()); }

			virtual size_t	GetSampleLength() const = 0;
			virtual float	GetLengthInSecond() const { return (float)GetSampleLength() / (float)GetFrequency(); }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) = 0;
		};
	}
}

#endif
