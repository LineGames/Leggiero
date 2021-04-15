////////////////////////////////////////////////////////////////////////////////
// Common/MultiPageFontSetting.h (Leggiero/Modules - Font)
//
// Font class definition for typesetting with multi-language pages setting
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__COMMON__MULTI_PAGE_FONT_SETTING_H
#define __LM_FONT__COMMON__MULTI_PAGE_FONT_SETTING_H


// Standard Library
#include <memory>
#include <tuple>
#include <vector>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Font
#include "FontTypes.h"


namespace Leggiero
{
	namespace Font
	{
		// Forward Declaration
		class IFontFace;


		// Font class definition for multiple language pages
		class MultiPageFontSetting
		{
		public:
			// Single font setting to be applied
			struct FontSetting
			{
			public:
				std::shared_ptr<IFontFace>	face;
				float						baselineOffsetRatio;
				float						scaleMultiply;
				float						horizontalRatioMultiply;
				bool						isForceBold;
				bool						isForceItalic;

				bool						isForceMonospacing;
				float						monospacingWidthRatio;

				bool						isPreferredForTofu;

				bool						isNeedRenderingCheck;
				//note: rendering check recommended for (codepoint >= 0xAC00 && codepoint < 0xD7A4) range(Hangul Syllables), due to for weired table of some fonts including Godo

			public:
				bool IsValid() const { return (bool)face; }

			public:
				FontSetting(std::shared_ptr<IFontFace> face, 
					float baselineOffsetRatio = 0.0f, float scaleMultiply = 1.0f, float horizontalRatioMultiply = 1.0f, 
					bool isForceBold = false, bool isForceItalic = false, bool isForceMonospacing = false, float monospacingWidthRatio = 0.0f, 
					bool isPreferredForTofu = false, bool isNeedRenderingCheck = false)
					: face(face), baselineOffsetRatio(baselineOffsetRatio), scaleMultiply(scaleMultiply), horizontalRatioMultiply(horizontalRatioMultiply)
					, isForceBold(isForceBold), isForceItalic(isForceItalic), isForceMonospacing(isForceMonospacing), monospacingWidthRatio(monospacingWidthRatio)
					, isPreferredForTofu(isPreferredForTofu), isNeedRenderingCheck(isNeedRenderingCheck)
				{ }

			public:
				static const FontSetting kInvalid;
			};

			// Range of page
			struct PageRange
			{
			public:
				GlyphCodePointType start;
				GlyphCodePointType finish;

			public:
				bool IsCodePointInRange(GlyphCodePointType codepoint) const { return ((codepoint >= start) && (codepoint <= finish)); }

			public:
				PageRange(GlyphCodePointType start, GlyphCodePointType finish)
					: start(start), finish(finish)
				{ }

				PageRange(const PageRange &other)
					: start(other.start), finish(other.finish)
				{ }

				PageRange(PageRange &&other)
					: start(other.start), finish(other.finish)
				{ }

			public:
				static const PageRange kAll;
				static const PageRange kNothing;
			};

		public:
			FontSetting GetSettingForGlyph(GlyphCodePointType codepoint) const;
			FontSetting GetTofuSetting() const;

			void RegisterSetting(const PageRange &applyingRange, const FontSetting &setting);

		protected:
			std::vector<std::tuple<PageRange, FontSetting> > m_fontSettings;
		};
	}
}

#endif
