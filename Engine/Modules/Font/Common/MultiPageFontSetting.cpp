////////////////////////////////////////////////////////////////////////////////
// Common/MultiPageFontSetting.cpp (Leggiero/Modules - Font)
//
// Multi-page font setting implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "MultiPageFontSetting.h"

// Standard Library
#include <limits>

// Leggiero.Font
#include "IFontFace.h"
#include "IGlyphDrawer.h"


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// MultiPageFontSetting

		//------------------------------------------------------------------------------
		// Static Values
		const MultiPageFontSetting::FontSetting MultiPageFontSetting::FontSetting::kInvalid(nullptr);
		const MultiPageFontSetting::PageRange MultiPageFontSetting::PageRange::kAll((GlyphCodePointType)0, std::numeric_limits<GlyphCodePointType>::max());
		const MultiPageFontSetting::PageRange MultiPageFontSetting::PageRange::kNothing((GlyphCodePointType)1, (GlyphCodePointType)0);

		//------------------------------------------------------------------------------
		MultiPageFontSetting::FontSetting MultiPageFontSetting::GetSettingForGlyph(GlyphCodePointType codepoint) const
		{
			for (const std::tuple<PageRange, FontSetting> &currentSettingEntry : m_fontSettings)
			{
				if (!std::get<0>(currentSettingEntry).IsCodePointInRange(codepoint))
				{
					continue;
				}

				if (!std::get<1>(currentSettingEntry).IsValid())
				{
					continue;
				}

				GlyphCodeInFont foundCharacterIndex = std::get<1>(currentSettingEntry).face->FindGlyphCodeInFont(codepoint);
				if (foundCharacterIndex == kGlyphCode_INVALID)
				{
					continue;
				}

				if (std::get<1>(currentSettingEntry).isNeedRenderingCheck)
				{
					IGlyphDrawer *testDrawer = std::get<1>(currentSettingEntry).face->GetBasicTestDrawer();
					if (testDrawer != nullptr)
					{
						IGlyphDrawer::GlyphSizeInfo effectiveDrawingSize = testDrawer->GetGlyphImageSIze(std::get<1>(currentSettingEntry).face, foundCharacterIndex, 72.0f);
						if (!effectiveDrawingSize.IsArea())
						{
							// Table Entry Exists, but there is no glyph
							continue;
						}
					}
				}

				return std::get<1>(currentSettingEntry);
			}

			return FontSetting::kInvalid;
		}

		//------------------------------------------------------------------------------
		MultiPageFontSetting::FontSetting MultiPageFontSetting::GetTofuSetting() const
		{
			for (const std::tuple<PageRange, FontSetting> &currentSettingEntry : m_fontSettings)
			{
				if (!std::get<0>(currentSettingEntry).IsCodePointInRange(0))
				{
					continue;
				}
				if (!std::get<1>(currentSettingEntry).isPreferredForTofu)
				{
					continue;
				}
				return std::get<1>(currentSettingEntry);
			}
			for (const std::tuple<PageRange, FontSetting> &currentSettingEntry : m_fontSettings)
			{
				if (!std::get<1>(currentSettingEntry).isPreferredForTofu)
				{
					continue;
				}
				return std::get<1>(currentSettingEntry);
			}
			for (const std::tuple<PageRange, FontSetting> &currentSettingEntry : m_fontSettings)
			{
				if (!std::get<0>(currentSettingEntry).IsCodePointInRange(0))
				{
					continue;
				}
				return std::get<1>(currentSettingEntry);
			}
			for (const std::tuple<PageRange, FontSetting> &currentSettingEntry : m_fontSettings)
			{
				return std::get<1>(currentSettingEntry);
			}

			return FontSetting::kInvalid;
		}

		//------------------------------------------------------------------------------
		void MultiPageFontSetting::RegisterSetting(const PageRange &applyingRange, const FontSetting &setting)
		{
			m_fontSettings.push_back(std::make_tuple(applyingRange, setting));
		}
	}
}
