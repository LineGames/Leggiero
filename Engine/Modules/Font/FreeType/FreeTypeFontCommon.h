////////////////////////////////////////////////////////////////////////////////
// FreeType/FreeTypeFontCommon.h (Leggiero/Modules - Font)
//
// Common Definitions for FreeType based Font Implementation
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__FREE_TYPE__FREE_TYPE_FONT_COMMON_H
#define __LM_FONT__FREE_TYPE__FREE_TYPE_FONT_COMMON_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <ft2build.h>
#include FT_FREETYPE_H

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>


namespace Leggiero
{
	namespace Font
	{
		namespace FreeType
		{
			void InitializeFreeTypeLibrary();
			void FinalizeFreeTypeLibrary();

			namespace _Internal
			{
				extern FT_Library							g_libraryInstance;
				extern Utility::Threading::SafePthreadLock	g_libraryFaceGenerateLock;

				constexpr float kFloatEpsilon = 0.000001f;

				constexpr float kBoldAmount = 0.05f;
				constexpr float kItalicAmount = 0.375f;
			}
		}
	}
}

#endif
