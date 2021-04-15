////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionFont.h (Leggiero/Modules - LegacyUI)
//
// UI Description about Font
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_FONT_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_FONT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.Font
#include <Font/Common/FontTypes.h>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Font
	{
		class MultiPageFontSetting;
	}


	namespace LUI
	{
		namespace Description
		{
			// Forward Declarations
			namespace Expression
			{
				template <typename ValueT>
				class IExpression;
			}

			namespace _Internal
			{
				struct FontEntryData;
			}


			// Font Class
			class FontClass
				: public IDescriptionObject
			{
			public:
				FontClass() { }
				virtual ~FontClass() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kFontClass; }

			public:
				static std::shared_ptr<FontClass> ReadFromFromXMLElement(ParsingXMLElementType *elem);

			public:
				Font::GlyphSizeType GetDefaultSize(DescriptionProcessingContext &processingContext);
				Graphics::GLByteARGB GetDefaultColor(DescriptionProcessingContext &processingContext);
				float GetDefaultStrokeWidth(DescriptionProcessingContext &processingContext);
				Graphics::GLByteARGB GetDefaultStrokeColor(DescriptionProcessingContext &processingContext);

				std::shared_ptr<Font::MultiPageFontSetting> GetFontSetting(DescriptionProcessingContext &processingContext);

			protected:
				std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_defaultSizeExpression;
				std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_defaultColorExpression;
				std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_defaultStrokeWidthExpression;
				std::shared_ptr<Expression::IExpression<ColorARGBValueType> >		m_defaultStrokeColorExpression;

				std::shared_ptr<Font::MultiPageFontSetting>	m_cachedFontSetting;

				std::vector<std::shared_ptr<_Internal::FontEntryData> >				m_classFontData;
			};
		}
	}
}

#endif
