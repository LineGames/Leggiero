////////////////////////////////////////////////////////////////////////////////
// Common/SingleFontSet.cpp (Leggiero/Modules - Font)
//
// Single Font Set Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "SingleFontSet.h"

// Leggiero.Font
#include "IFontFace.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// SingleFontSet

		//------------------------------------------------------------------------------
		SingleFontSet::SingleFontSet(std::shared_ptr<IFontFace> face)
			: m_face(face)
		{

		}

		//------------------------------------------------------------------------------
		IFontSet::EffectiveFontResult SingleFontSet::GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style) const
		{
			if (!m_face)
			{
				return IFontSet::EffectiveFontResult();
			}
			
			return IFontSet::EffectiveFontResult(m_face, m_face->FindGlyphCodeInFont(glyph));
		}
	}
}
