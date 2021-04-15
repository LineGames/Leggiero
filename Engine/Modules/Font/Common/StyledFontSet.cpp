////////////////////////////////////////////////////////////////////////////////
// Common/StyledFontSet.cpp (Leggiero/Modules - Font)
//
// Implementation of font set with styled fonts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "StyledFontSet.h"

// Leggiero.Font
#include "IFontFace.h"
#include "SingleFontSet.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// StyledFontSet

			//------------------------------------------------------------------------------
		StyledFontSet::StyledFontSet(std::shared_ptr<IFontSet> baseSet, std::shared_ptr<IFontSet> boldSet, std::shared_ptr<IFontSet> italicSet, std::shared_ptr<IFontSet> boldItalicSet)
			: m_baseSet(baseSet), m_boldSet(boldSet), m_italicSet(italicSet), m_boldItalicSet(boldItalicSet)
		{

		}

		//------------------------------------------------------------------------------
		StyledFontSet::StyledFontSet(std::shared_ptr<IFontFace> baseFace, std::shared_ptr<IFontFace> boldFace, std::shared_ptr<IFontFace> italicFace, std::shared_ptr<IFontFace> boldItalicFace)
			: m_baseSet(baseFace ? std::make_shared<SingleFontSet>(baseFace) : nullptr)
			, m_boldSet(boldFace ? std::make_shared<SingleFontSet>(boldFace) : nullptr)
			, m_italicSet(italicFace ? std::make_shared<SingleFontSet>(italicFace) : nullptr)
			, m_boldItalicSet(boldItalicFace ? std::make_shared<SingleFontSet>(boldItalicFace) : nullptr)
		{

		}

		//------------------------------------------------------------------------------
		IFontSet::EffectiveFontResult StyledFontSet::GetEffectiveFont(GlyphCodePointType glyph, GlyphStyleType style) const
		{
			std::shared_ptr<IFontSet> firstNonNull;
			bool isFirstNonNullNeedManualBold = false;
			bool isFirstNonNullNeedManualItalic = false;

			auto _FontCheckFunc = [glyph, style, &firstNonNull, &isFirstNonNullNeedManualBold, &isFirstNonNullNeedManualItalic](std::shared_ptr<IFontSet> targetSet, bool isBoldUnresolved, bool isItalicUnresolved) {
				if (targetSet)
				{
					IFontSet::EffectiveFontResult currentSetResult = targetSet->GetEffectiveFont(glyph, style);
					if (currentSetResult.IsValid())
					{
						if (!currentSetResult.IsTofu())
						{
							currentSetResult.isBoldUnresolved = isBoldUnresolved;
							currentSetResult.isItalicUnresolved = isItalicUnresolved;
							return currentSetResult;
						}
						if (!firstNonNull)
						{
							firstNonNull = targetSet;
							isFirstNonNullNeedManualBold = isBoldUnresolved;
							isFirstNonNullNeedManualItalic = isItalicUnresolved;
						}
					}
				}
				return IFontSet::EffectiveFontResult();
			};

			IFontSet::EffectiveFontResult tempResult;
			if (!Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kBold) && !Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kItalic))
			{
				// Regular
				tempResult = _FontCheckFunc(m_baseSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_boldSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_italicSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_boldItalicSet, false, false); if (tempResult.IsValid()) return tempResult;
			}
			else if (!Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kItalic))
			{
				// Bold
				tempResult = _FontCheckFunc(m_boldSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_baseSet, true, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_boldItalicSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_italicSet, true, false); if (tempResult.IsValid()) return tempResult;
			}
			else if (!Utility::SyntacticSugar::HasFlag(style, GlyphStyleType::kBold))
			{
				// Italic
				tempResult = _FontCheckFunc(m_italicSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_boldItalicSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_baseSet, false, true); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_boldSet, false, true); if (tempResult.IsValid()) return tempResult;
			}
			else
			{
				// BoldItalic
				tempResult = _FontCheckFunc(m_boldItalicSet, false, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_italicSet, true, false); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_boldSet, false, true); if (tempResult.IsValid()) return tempResult;
				tempResult = _FontCheckFunc(m_baseSet, true, true); if (tempResult.IsValid()) return tempResult;
			}

			if (firstNonNull)
			{
				// Tofu
				IFontSet::EffectiveFontResult tofuResult = firstNonNull->GetEffectiveFont(glyph, style);
				tofuResult.isBoldUnresolved = isFirstNonNullNeedManualBold;
				tofuResult.isItalicUnresolved = isFirstNonNullNeedManualItalic;
				return tofuResult;
			}

			return IFontSet::EffectiveFontResult();
		}
	}
}
