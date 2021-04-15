////////////////////////////////////////////////////////////////////////////////
// Common/SoundTypes.cpp (Leggiero/Modules - Sound)
//
// Basic Implementations for Sound Types
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		//------------------------------------------------------------------------------
		size_t GetSampleByteSize(SampleFormat format)
		{
			switch (format)
			{
				case SampleFormat::kMono_8: return 1;
				case SampleFormat::kMono_16: return 2;
				case SampleFormat::kStereo_8: return 2;
				case SampleFormat::kStereo_16: return 4;
			}
			return 0;
		}

		//------------------------------------------------------------------------------
		int GetChannelNumber(SampleFormat format)
		{
			switch (format)
			{
				case SampleFormat::kMono_8: return 1;
				case SampleFormat::kMono_16: return 1;
				case SampleFormat::kStereo_8: return 2;
				case SampleFormat::kStereo_16: return 2;
			}
			return 0;
		}
	}
}
