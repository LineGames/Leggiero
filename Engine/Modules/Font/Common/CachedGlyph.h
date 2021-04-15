////////////////////////////////////////////////////////////////////////////////
// Common/CachedGlyph.h (Leggiero/Modules - Font)
//
// Cached Glyph Entry
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__CACHED_GLYPH_H
#define __LM_FONT__COMMON__CACHED_GLYPH_H


// Standard Library
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Font
#include "FontTypes.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class RuntimeTextureAtlasEntry;
	}

	namespace Font
	{
		// Forward Declarations
		class IFontFace;
		class GlyphCache;


		// Cached Glyph Entry
		class CachedGlyph
			: private Utility::SyntacticSugar::NonCopyable
		{
			friend class GlyphCache;

		public:
			virtual ~CachedGlyph();

		protected:
			// Only can be created by glyph cache
			CachedGlyph(bool isDrawSuccess, GLint offsetX, GLint offsetY, int advanceX, int advanceY, std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> image,
				std::shared_ptr<IFontFace> effectiveFace, GlyphCodeInFont glyphCode, float horizontalRatio, bool isManualBold, bool isManualItalic);

		private:
			CachedGlyph() = delete;

		public:
			bool IsLoadSuccess() const { return m_isDrawSuccess; }

			bool HasImage() const { return (bool)m_image; }
			std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> GetFontImage() const { return m_image; }

			GLint GetOffsetX() const { return m_offsetX; }
			GLint GetOffsetY() const { return m_offsetY; }

			GLint GetAdvanceX() const { return m_advanceX; }
			GLint GetAdvanceY() const { return m_advanceY; }

			std::shared_ptr<IFontFace> GetEffectiveFontFace() const { return m_effectiveFace; }
			GlyphCodeInFont GetGlyphCode() const { return m_glyphCode; }
			float GetHorizontalRatio() const { return m_horizontalRatio; }
			bool IsManualBoldGlyph() const { return m_isManualBold; }
			bool IsManualItalicGlyph() const { return m_isManualItalic; }

		protected:
			bool m_isDrawSuccess;

			GLint m_offsetX;
			GLint m_offsetY;

			int m_advanceX;
			int m_advanceY;

			std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> m_image;

			std::shared_ptr<IFontFace> m_effectiveFace;
			GlyphCodeInFont m_glyphCode;
			float m_horizontalRatio;
			bool m_isManualBold;
			bool m_isManualItalic;
		};
	}
}

#endif
