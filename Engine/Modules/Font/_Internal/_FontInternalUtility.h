////////////////////////////////////////////////////////////////////////////////
// _Internal/_FontInternalUtility.h (Leggiero/Modules - Font)
//
// Internal Utility Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT___INTERNAL___FONT_INTERNAL_UTILITY_H
#define __LM_FONT___INTERNAL___FONT_INTERNAL_UTILITY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Font
#include "../Common/FontTypes.h"


namespace Leggiero
{
	namespace Font
	{
		namespace _Internal
		{
			// Issue Unique Font Id
			FontIdType IssueFontId();

			constexpr float kFontSizeDetail = 0.0001f;
			constexpr float kFontSizeRatioDetail = 0.000001f;
		}
	}
}

#endif
