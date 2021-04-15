////////////////////////////////////////////////////////////////////////////////
// Common/IFontSet.cpp (Leggiero/Modules - Font)
//
// Common Implementations for Font Set
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "IFontSet.h"

// Leggiero.Font
#include "../_Internal/_FontInternalUtility.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// Special Font Set

		namespace _Internal
		{
			// Empty Font Set Class
			class NullFontSet
				: public IFontSet
			{
			public:
				virtual IFontSet::EffectiveFontResult GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style) const override
				{
					return IFontSet::EffectiveFontResult();
				}

				virtual bool IsValidFontSet() const override { return false; }
			};
		}


		//////////////////////////////////////////////////////////////////////////////// IFontSet

		const std::shared_ptr<IFontSet> IFontSet::kNullFontSet(std::dynamic_pointer_cast<IFontSet>(std::make_shared<_Internal::NullFontSet>()));

		//------------------------------------------------------------------------------
		IFontSet::IFontSet()
			: m_setUniqueId(_Internal::IssueFontId())
		{

		}
	}
}
