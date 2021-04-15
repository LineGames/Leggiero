////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALCommon.h (Leggiero/Modules - Sound)
//
// Common Utility for OpenAL Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OPENAL_BACKEND__OPENAL_COMMON_H
#define __LM_SOUND__OPENAL_BACKEND__OPENAL_COMMON_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <OpenAL.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			// Convert Audio Sample Format to OpenAL's
			ALenum ToOpenALFormatEnum(SampleFormat format);
		}
	}
}

#endif
