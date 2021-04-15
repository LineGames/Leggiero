////////////////////////////////////////////////////////////////////////////////
// Common/SingleFontSet.h (Leggiero/Modules - Font)
//
// Font Set with a single Font Face
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__SINGLE_FONT_SET_H
#define __LM_FONT__COMMON__SINGLE_FONT_SET_H


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
		// Forward Declaration
		class IFontFace;


		// Single Font Set
		class SingleFontSet
			: public IFontSet
		{
		public:
			SingleFontSet(std::shared_ptr<IFontFace> face);
			virtual ~SingleFontSet() { }

		private:
			SingleFontSet() = delete;

		public:
			// Get a real font to render a given glyph
			virtual EffectiveFontResult GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style = GlyphStyleType::kNone) const override;

			// Get whther the set is a valid font set
			virtual bool IsValidFontSet() const override { return (bool)m_face; }

		protected:
			std::shared_ptr<IFontFace> m_face;
		};
	}
}

#endif
