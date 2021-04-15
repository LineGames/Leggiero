////////////////////////////////////////////////////////////////////////////////
// Common/IGlyphDrawer.h (Leggiero/Modules - Font)
//
// Interface for Drawer which can generate Bitmap Image for a glyph
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__I_GLYPH_DRAWER_H
#define __LM_FONT__COMMON__I_GLYPH_DRAWER_H


// Standard Library
#include <memory>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.Font
#include "FontTypes.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declarations
		class IFontFace;


		// Common Interface for Glyph Drawer
		class IGlyphDrawer
		{
		public:
			// Type to store size information of a glyph
			struct GlyphSizeInfo
			{
			public:
				GLsizei width;
				GLsizei height;

				int xAdvance;
				int yAdvance;

			protected:
				bool m_isValid;

			public:
				bool IsValid() const { return m_isValid; }
				bool IsArea() const { return ((width > 0) && (height > 0)); }

			public:
				GlyphSizeInfo(bool isValid, GLsizei width = 0, GLsizei height = 0, int xAdvance = 0, int yAdvance = 0)
					: m_isValid(isValid), width(width), height(height), xAdvance(xAdvance), yAdvance(yAdvance)
				{ }
			};

			// Glyph Drawing Result
			struct GlyphDrawResult
			{
			public:
				bool isDrawSuccess;

				GLsizei realDrawWidth;
				GLsizei realDrawHeight;

				GLint bitmapOffsetX;
				GLint bitmapOffsetY;

				int xAdvance;
				int yAdvance;

			public:
				GlyphDrawResult(bool isSuccess, GLsizei realDrawWidth = 0, GLsizei realDrawHeight = 0, GLint bitmapOffsetX = 0, GLint bitmapOffsetY = 0, int xAdvance = 0, int yAdvance = 0)
					: isDrawSuccess(isSuccess), realDrawWidth(realDrawWidth), realDrawHeight(realDrawHeight), bitmapOffsetX(bitmapOffsetX), bitmapOffsetY(bitmapOffsetY), xAdvance(xAdvance), yAdvance(yAdvance)
				{ }
			};

		public:
			// Draw bitmap image of a glyph on a given image buffer
			virtual GlyphDrawResult DrawGlyph(void *imageBuffer, std::shared_ptr<IFontFace> font, GlyphCodeInFont glyphCode, GlyphSizeType size, float horizontalRatio = 1.0f,
				Graphics::GLByteARGB::ColorCodeType color = Graphics::GLByteARGB::kWhite.ToColorCodeARGB(), bool isManualBold = false, bool isManualItalic = false,
				GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, bool isPremultipliedAlpha = true,
				float strokeWidth = 0.0f, Graphics::GLByteARGB::ColorCodeType strokeColor = Graphics::GLByteARGB::kTransparent.ToColorCodeARGB()) = 0;

			// Get size information to prepare glyph drawing
			virtual GlyphSizeInfo GetGlyphImageSIze(std::shared_ptr<IFontFace> font, GlyphCodeInFont glyphCode, GlyphSizeType size, float horizontalRatio = 1.0f,
				bool isManualBold = false, bool isManualItalic = false,
				float strokeWidth = 0.0f) = 0;
		};
	}
}

#endif
