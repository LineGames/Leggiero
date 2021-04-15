////////////////////////////////////////////////////////////////////////////////
// Common/StyledFontSet.h (Leggiero/Modules - Font)
//
// Font set with styled fonts
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__STYLED_FONT_SET_H
#define __LM_FONT__COMMON__STYLED_FONT_SET_H


// Standard Library
#include <memory>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Font
#include "FontTypes.h"
#include "IFontSet.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declarations
		class IFontFace;


		// Font Set with Styled Fonts
		class StyledFontSet
			: public IFontSet
		{
		public:
			StyledFontSet(std::shared_ptr<IFontSet> baseSet, std::shared_ptr<IFontSet> boldSet = nullptr, std::shared_ptr<IFontSet> italicSet = nullptr, std::shared_ptr<IFontSet> boldItalicSet = nullptr);
			StyledFontSet(std::shared_ptr<IFontFace> baseFace, std::shared_ptr<IFontFace> boldFace = nullptr, std::shared_ptr<IFontFace> italicFace = nullptr, std::shared_ptr<IFontFace> boldItalicFace = nullptr);

			virtual ~StyledFontSet() { }

		private:
			StyledFontSet() = delete;

		public:
			// Get a real font to render a given glyph
			virtual EffectiveFontResult GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style = GlyphStyleType::kNone) const override;

		protected:
			std::shared_ptr<IFontSet> m_baseSet;
			std::shared_ptr<IFontSet> m_boldSet;
			std::shared_ptr<IFontSet> m_italicSet;
			std::shared_ptr<IFontSet> m_boldItalicSet;
		};
	}
}

#endif
