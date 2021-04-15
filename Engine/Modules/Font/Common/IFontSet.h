////////////////////////////////////////////////////////////////////////////////
// Common/IFontSet.h (Leggiero/Modules - Font)
//
// Interface for a logical set of fonts
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__I_FONT_SET_H
#define __LM_FONT__COMMON__I_FONT_SET_H


// Standard Library
#include <memory>

// Leggiero.Font
#include "FontTypes.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declarations
		class IFontFace;


		// Font Set Interface
		class IFontSet
		{
		public:
			IFontSet();
			virtual ~IFontSet() { }

		public:
			// Individual Font Search Result for a glyph
			struct EffectiveFontResult
			{
			public:
				std::shared_ptr<IFontFace>	face;
				GlyphCodeInFont				glyphCode;

				bool						isBoldUnresolved;
				bool						isItalicUnresolved;

			public:
				bool IsValid() const { return ((bool)face); }
				bool IsTofu() const { return (glyphCode == kGlyphCode_INVALID); }

			public:
				EffectiveFontResult(std::shared_ptr<IFontFace> face = nullptr, GlyphCodeInFont glyphCode = 0, bool isBoldUnresolved = false, bool isItalicUnresolved = false)
					: face(face), glyphCode(glyphCode), isBoldUnresolved(isBoldUnresolved), isItalicUnresolved(isItalicUnresolved)
				{ }
			};

		public:
			// Get Unique Id for the Font Set
			FontIdType GetSetUniqueId() const { return m_setUniqueId; }

			// Get a real font to render a given glyph
			virtual EffectiveFontResult GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style = GlyphStyleType::kNone) const = 0;

			// Get whther the set is a valid font set
			virtual bool IsValidFontSet() const { return true; }

		public:
			// Empty Font Set which have NO Font
			static const std::shared_ptr<IFontSet> kNullFontSet;

		protected:
			FontIdType m_setUniqueId;
		};
	}
}

#endif
