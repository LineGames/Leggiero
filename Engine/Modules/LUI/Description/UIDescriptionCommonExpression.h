////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionCommonExpression.h (Leggiero/Modules - LegacyUI)
//
// Commonly used expression objects in UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_COMMON_EXPRESSION_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_COMMON_EXPRESSION_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"
#include "UIDescriptionExpression.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			// Variable stores scaled value
			template <typename ValueT>
			class ScaledVariable
				: public Variable<ValueT>
			{
			public:
				ScaledVariable() : m_quantize(QuantizeType::kNo) { }
				virtual ~ScaledVariable() { }

			protected:	// Variable<ValueT>
				virtual void _Evaluate(DescriptionProcessingContext &processingContext) override;

			public:
				static std::shared_ptr<ScaledVariable<ValueT> > ReadFromFromXMLElement(ParsingXMLElementType *elem);

			protected:
				std::shared_ptr<Expression::IExpression<ValueT> >					m_valueExpression;
				std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	m_scaleExpression;

				QuantizeType										m_quantize;
				std::shared_ptr<Expression::IExpression<ValueT> >	m_quantizeUnitExpression;
			};

			template<> void ScaledVariable<IntegerValueType>::_Evaluate(DescriptionProcessingContext &processingContext);
			template<> void ScaledVariable<FloatingPointValueType>::_Evaluate(DescriptionProcessingContext &processingContext);

			template<> std::shared_ptr<ScaledVariable<IntegerValueType> > ScaledVariable<IntegerValueType>::ReadFromFromXMLElement(ParsingXMLElementType *elem);
			template<> std::shared_ptr<ScaledVariable<FloatingPointValueType> > ScaledVariable<FloatingPointValueType>::ReadFromFromXMLElement(ParsingXMLElementType *elem);
		}
	}
}

#endif
