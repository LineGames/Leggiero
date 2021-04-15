////////////////////////////////////////////////////////////////////////////////
// Common/SoundTypes.h (Leggiero/Modules - Sound)
//
// Common Type Definitions for Sound Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__COMMON__SOUND_TYPES_H
#define __LM_SOUND__COMMON__SOUND_TYPES_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>
#include <cstring>


namespace Leggiero
{
	namespace Sound
	{
		// Type to represent sample number in sound
		using SampleNumberType = int64_t;

		// Integer Sampling Frequency(like 44100)
		using SamplingFrequencyType = int32_t;

		// Supporting Sound Sample Data Format
		enum class SampleFormat
		{
			kINVALID, 

			kMono_8, 
			kStereo_8,
			kMono_16, 
			kStereo_16, 
		};

		// Get single sample data byte size
		size_t GetSampleByteSize(SampleFormat format);

		// Get channel number of given format
		int GetChannelNumber(SampleFormat format);
	}
}

#endif
