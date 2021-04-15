////////////////////////////////////////////////////////////////////////////////
// Common/PriorityFontSet.cpp (Leggiero/Modules - Font)
//
// Prioritized Fonts Set Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "PriorityFontSet.h"

// Leggiero.Font
#include "IFontFace.h"
#include "SingleFontSet.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// PriorityFontSet

		//------------------------------------------------------------------------------
		PriorityFontSet::PriorityFontSet(std::shared_ptr<IFontSet> baseSet)
		{
			m_baseSets.push_back(baseSet);
		}

		//------------------------------------------------------------------------------
		PriorityFontSet::PriorityFontSet(std::shared_ptr<IFontFace> baseFace)
		{
			m_baseSets.push_back(std::make_shared<SingleFontSet>(baseFace));
		}

		//------------------------------------------------------------------------------
		PriorityFontSet::PriorityFontSet(std::initializer_list<std::shared_ptr<IFontSet> > initializer)
			: m_baseSets(initializer)
		{
		}

		//------------------------------------------------------------------------------
		PriorityFontSet::PriorityFontSet(std::initializer_list<std::shared_ptr<IFontFace> > initializer)
		{
			m_baseSets.reserve(initializer.size());
			for (const std::shared_ptr<IFontFace> &baseFace : initializer)
			{
				m_baseSets.push_back(std::make_shared<SingleFontSet>(baseFace));
			}
		}

		//------------------------------------------------------------------------------
		IFontSet::EffectiveFontResult PriorityFontSet::GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style) const
		{
			if (m_baseSets.empty())
			{
				return IFontSet::EffectiveFontResult();
			}

			IFontSet::EffectiveFontResult firstValidResult;
			for (const std::shared_ptr<IFontSet> &baseSet : m_baseSets)
			{
				IFontSet::EffectiveFontResult currentSetResult = baseSet->GetEffectiveFont(glyph, style);
				if (currentSetResult.IsValid())
				{
					if (!currentSetResult.IsTofu())
					{
						return currentSetResult;
					}
					firstValidResult = currentSetResult;
				}
			}

			return firstValidResult;
		}
	}
}
