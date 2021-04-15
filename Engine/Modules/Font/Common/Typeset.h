////////////////////////////////////////////////////////////////////////////////
// Common/Typeset.h (Leggiero/Modules - Font)
//
// Typeset Functions
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__TYPESET_H
#define __LM_FONT__COMMON__TYPESET_H


// Standard Library
#include <memory>
#include <string>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Graphics
#include <Graphics/Common/GraphicsTypes.h>
#include <Graphics/Common/GLColor.h>

// Leggiero.Font
#include "FontTypes.h"
#include "Typesetting.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declarations
		class GlyphManagerComponent;
		class IFontSet;
		class MultiPageFontSetting;


		// Text Alignment
		enum class TextAlignType
		{
			kDefault,

			kLeft,
			kCenter,
			kRight,

			kJustify,		// Balanced Alignment for Characters
			kJustifyWord,	// Balanced Alignment for Words
		};

		TextAlignType ParseTextAlignType(const char *alignmentString);


		// Typeset one line horizontal text for given font set
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalSingleLine(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<IFontSet> fontSet, GlyphSizeType size, GlyphStyleType style = GlyphStyleType::kNone,
			Graphics::HorizontalAlignType horizontalAlign = Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType verticalAlign = Graphics::VerticalAlignType::kTop,
			Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite, float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent,
			bool isFitGrid = true, TextureType textureType = TextureType::kNormal);

		// Typeset one line horizontal text for given multi-page font setting
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalSingleLine(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<MultiPageFontSetting> fontSettting, GlyphSizeType size,
			Graphics::HorizontalAlignType horizontalAlign = Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType verticalAlign = Graphics::VerticalAlignType::kTop,
			Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite, float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent,
			bool isFitGrid = true, TextureType textureType = TextureType::kNormal);

		// Typeset given width text block for given font set
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalBlock(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<IFontSet> fontSet, GlyphSizeType size, Graphics::BasicCoordType blockWidth, GlyphStyleType style = GlyphStyleType::kNone,
			TextAlignType textAlign = TextAlignType::kLeft, TextAlignType lastLineTextAlign = TextAlignType::kDefault, float lineSpacing = 1.5f,
			Graphics::HorizontalAlignType horizontalAlign = Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType verticalAlign = Graphics::VerticalAlignType::kTop,
			Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite, float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent,
			bool isFitGrid = true, TextureType textureType = TextureType::kNormal, bool isWordAwareLineBreak = true, bool isIgnoreLineBreak = false);

		// Typeset given width text block for given multi-page font setting
		std::shared_ptr<CachedGlyphTypesetting> TypesetCachedGlyphesHorizontalBlock(GlyphManagerComponent *glyphManager, GameDataString text,
			std::shared_ptr<MultiPageFontSetting> fontSettting, GlyphSizeType size, Graphics::BasicCoordType blockWidth,
			TextAlignType textAlign = TextAlignType::kLeft, TextAlignType lastLineTextAlign = TextAlignType::kDefault, float lineSpacing = 1.5f,
			Graphics::HorizontalAlignType horizontalAlign = Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType verticalAlign = Graphics::VerticalAlignType::kTop,
			Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite, float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent,
			bool isFitGrid = true, TextureType textureType = TextureType::kNormal, bool isWordAwareLineBreak = true, bool isIgnoreLineBreak = false);
	}
}

#endif
