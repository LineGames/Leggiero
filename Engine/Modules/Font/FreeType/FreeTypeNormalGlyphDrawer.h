////////////////////////////////////////////////////////////////////////////////
// FreeType/FreeTypeNormalGlyphDrawer.h (Leggiero/Modules - Font)
//
// FreeType based Glyph Drawer for normal bitmap texture
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__FREE_TYPE__FREE_TYPE_NORMAL_GLYPH_DRAWER_H
#define __LM_FONT__FREE_TYPE__FREE_TYPE_NORMAL_GLYPH_DRAWER_H


// External Library
#include <ft2build.h>
#include FT_FREETYPE_H

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Font
#include "../Common/FontTypes.h"
#include "../Common/IGlyphDrawer.h"


namespace Leggiero
{
	namespace Font
	{
		namespace FreeType
		{
			// FreeType based Normal Glyph Drawer
			class NormalGlyphDrawer
				: public IGlyphDrawer
			{
			public:
				NormalGlyphDrawer();
				virtual ~NormalGlyphDrawer();
				
			public:
				// Draw bitmap image of a glyph on a given image buffer
				virtual GlyphDrawResult DrawGlyph(void *imageBuffer, std::shared_ptr<IFontFace> font, GlyphCodeInFont glyphCode, GlyphSizeType size, float horizontalRatio = 1.0f,
					Graphics::GLByteARGB::ColorCodeType color = Graphics::GLByteARGB::kWhite.ToColorCodeARGB(), bool isManualBold = false, bool isManualItalic = false,
					GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, bool isPremultipliedAlpha = true,
					float strokeWidth = 0.0f, Graphics::GLByteARGB::ColorCodeType strokeColor = Graphics::GLByteARGB::kTransparent.ToColorCodeARGB()) override;

				// Get size information to prepare glyph drawing
				virtual GlyphSizeInfo GetGlyphImageSIze(std::shared_ptr<IFontFace> font, GlyphCodeInFont glyphCode, GlyphSizeType size, float horizontalRatio = 1.0f,
					bool isManualBold = false, bool isManualItalic = false,
					float strokeWidth = 0.0f) override;
			};
		}
	}
}

#endif
