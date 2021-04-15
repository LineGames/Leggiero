////////////////////////////////////////////////////////////////////////////////
// Common/FontTypes.h (Leggiero/Modules - Font)
//
// Definitions of Font related Types
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__FONT_TYPES_H
#define __LM_FONT__COMMON__FONT_TYPES_H


// Standard Library
#include <cstdint>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <utfcpp/utf8.h>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>


namespace Leggiero
{
	namespace Font
	{
		// Unicode Code Point
		using GlyphCodePointType = utf8::uint32_t;

		// Unique Font Id Type
		using FontIdType = uint32_t;

		// Glyph code in a specific font
		using GlyphCodeInFont = uint32_t;
		constexpr GlyphCodeInFont kGlyphCode_INVALID = 0;

		// Size of Glyph
		using GlyphSizeType = float;

		// Glyph Style Flags
		enum class GlyphStyleType
		{
			kNone = 0x0,

			kBold = 0x1,
			kItalic = 0x2,

			kPremultipliedAlpha = 0x4,

			kBoldItalic = (kBold | kItalic),
		};
		MAKE_ENUM_FLAG(GlyphStyleType);

		// Font Texture Type for Rendering Use
		enum class TextureType : size_t
		{
			kNormal,

			kTextureTypeCount,
		};
	}
}

#endif
