////////////////////////////////////////////////////////////////////////////////
// Common/CachedGlyph.cpp (Leggiero/Modules - Font)
//
// Cached Glyph Entry Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CachedGlyph.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// CachedGlyph

		//------------------------------------------------------------------------------
		CachedGlyph::CachedGlyph(bool isDrawSuccess, GLint offsetX, GLint offsetY, int advanceX, int advanceY, std::shared_ptr<Graphics::RuntimeTextureAtlasEntry> image,
			std::shared_ptr<IFontFace> effectiveFace, GlyphCodeInFont glyphCode, float horizontalRatio, bool isManualBold, bool isManualItalic)
			: m_image(image), m_isDrawSuccess(isDrawSuccess), m_offsetX(offsetX), m_offsetY(offsetY), m_advanceX(advanceX), m_advanceY(advanceY)
			, m_effectiveFace(effectiveFace), m_glyphCode(glyphCode), m_horizontalRatio(horizontalRatio), m_isManualBold(isManualBold), m_isManualItalic(isManualItalic)
		{
		}

		//------------------------------------------------------------------------------
		CachedGlyph::~CachedGlyph()
		{
		}
	}
}
