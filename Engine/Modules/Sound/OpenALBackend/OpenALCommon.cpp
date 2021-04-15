////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALCommon.cpp (Leggiero/Modules - Sound)
//
// OpenAL Common Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OpenALCommon.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			//------------------------------------------------------------------------------
			// Convert Audio Sample Format to OpenAL's
			ALenum ToOpenALFormatEnum(SampleFormat format)
			{
				switch (format)
				{
					case SampleFormat::kMono_8: return AL_FORMAT_MONO8;
					case SampleFormat::kMono_16: return AL_FORMAT_MONO16;
					case SampleFormat::kStereo_8: return AL_FORMAT_STEREO8;
					case SampleFormat::kStereo_16: return AL_FORMAT_STEREO16;
				}
				return AL_NONE;
			}
		}
	}
}
