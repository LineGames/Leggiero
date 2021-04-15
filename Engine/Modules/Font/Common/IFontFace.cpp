////////////////////////////////////////////////////////////////////////////////
// Common/IFontFace.cpp (Leggiero/Modules - Font)
//
// Abstract Font Face Basic Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "IFontFace.h"

// Leggiero.Font
#include "../_Internal/_FontInternalUtility.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// IFontFace

		//------------------------------------------------------------------------------
		IFontFace::IFontFace()
			: m_fontId(_Internal::IssueFontId())
		{
		}
	}
}
