////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionCommonExpression.cpp (Leggiero/Modules - LegacyUI)
//
// Common expression helper implementation for UI Description
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionCommonExpression.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>

// Leggiero.LegacyUI
#include "UIDescriptionManager.h"
#include "UIDescriptionProcessingContext.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// ScaledVariable

			//------------------------------------------------------------------------------
			template<>
			void ScaledVariable<IntegerValueType>::_Evaluate(DescriptionProcessingContext &processingContext)
			{
				IntegerValueType currentValue = m_valueExpression->Evaluate(processingContext);

				if (m_scaleExpression)
				{
					if (m_quantize != QuantizeType::kNo)
					{
						if (m_quantizeUnitExpression)
						{
							IntegerValueType unitValue = m_quantizeUnitExpression->Evaluate(processingContext);
							switch (m_quantize)
							{
								case QuantizeType::kFloor: currentValue = (IntegerValueType)(floor((float)(currentValue * m_scaleExpression->Evaluate(processingContext)) / (float)unitValue)) * unitValue; break;
								case QuantizeType::kRound: currentValue = (IntegerValueType)(round((float)(currentValue * m_scaleExpression->Evaluate(processingContext)) / (float)unitValue)) * unitValue; break;
								case QuantizeType::kCeil: currentValue = (IntegerValueType)(ceil((float)(currentValue * m_scaleExpression->Evaluate(processingContext)) / (float)unitValue)) * unitValue; break;
							}
						}
						else
						{
							switch (m_quantize)
							{
								case QuantizeType::kFloor: currentValue = (IntegerValueType)(floor(currentValue * m_scaleExpression->Evaluate(processingContext))); break;
								case QuantizeType::kRound: currentValue = (IntegerValueType)(round(currentValue * m_scaleExpression->Evaluate(processingContext))); break;
								case QuantizeType::kCeil: currentValue = (IntegerValueType)(ceil(currentValue * m_scaleExpression->Evaluate(processingContext))); break;
							}
						}
					}
				}
				else
				{
					if (m_quantize != QuantizeType::kNo)
					{
						if (m_quantizeUnitExpression)
						{
							IntegerValueType unitValue = m_quantizeUnitExpression->Evaluate(processingContext);
							switch (m_quantize)
							{
								case QuantizeType::kFloor: currentValue = (IntegerValueType)(floor((float)currentValue / (float)unitValue)) * unitValue; break;
								case QuantizeType::kRound: currentValue = (IntegerValueType)(round((float)currentValue / (float)unitValue)) * unitValue; break;
								case QuantizeType::kCeil: currentValue = (IntegerValueType)(ceil((float)currentValue / (float)unitValue)) * unitValue; break;
							}
						}
						else
						{
							switch (m_quantize)
							{
								case QuantizeType::kFloor: currentValue = (IntegerValueType)(floor(currentValue)); break;
								case QuantizeType::kRound: currentValue = (IntegerValueType)(round(currentValue)); break;
								case QuantizeType::kCeil: currentValue = (IntegerValueType)(ceil(currentValue)); break;
							}
						}
					}
				}

				m_value = currentValue;
				m_isEvaluated = true;
			}

			//------------------------------------------------------------------------------
			template<>
			void ScaledVariable<FloatingPointValueType>::_Evaluate(DescriptionProcessingContext &processingContext)
			{
				FloatingPointValueType currentValue = m_valueExpression->Evaluate(processingContext);

				if (m_scaleExpression)
				{
					currentValue *= m_scaleExpression->Evaluate(processingContext);
				}

				if (m_quantize != QuantizeType::kNo)
				{
					if (m_quantizeUnitExpression)
					{
						FloatingPointValueType unitValue = m_quantizeUnitExpression->Evaluate(processingContext);
						switch (m_quantize)
						{
							case QuantizeType::kFloor: currentValue = (FloatingPointValueType)(floor(currentValue / unitValue)) * unitValue; break;
							case QuantizeType::kRound: currentValue = (FloatingPointValueType)(round(currentValue / unitValue)) * unitValue; break;
							case QuantizeType::kCeil: currentValue = (FloatingPointValueType)(ceil(currentValue / unitValue)) * unitValue; break;
						}
					}
					else
					{
						switch (m_quantize)
						{
							case QuantizeType::kFloor: currentValue = (FloatingPointValueType)(floor(currentValue)); break;
							case QuantizeType::kRound: currentValue = (FloatingPointValueType)(round(currentValue)); break;
							case QuantizeType::kCeil: currentValue = (FloatingPointValueType)(ceil(currentValue)); break;
						}
					}
				}

				m_value = currentValue;
				m_isEvaluated = true;
			}

			//------------------------------------------------------------------------------
			template<>
			std::shared_ptr<ScaledVariable<IntegerValueType> > ScaledVariable<IntegerValueType>::ReadFromFromXMLElement(tinyxml2::XMLElement *elem)
			{
				const char *valueAttribute = elem->Attribute("value");
				if (valueAttribute == nullptr)
				{
					// No Value
					return nullptr;
				}

				std::shared_ptr<ScaledVariable<IntegerValueType> > createdDescription(std::make_shared<ScaledVariable<IntegerValueType> >());
				createdDescription->m_valueExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(valueAttribute, UIDescriptionValueType::kInteger));

				const char *scaleAttribute = elem->Attribute("scale");
				if (scaleAttribute != nullptr)
				{
					createdDescription->m_scaleExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(scaleAttribute, UIDescriptionValueType::kFloatingPoint));
				}

				const char *quantizeAttribute = elem->Attribute("quantize");
				if (quantizeAttribute != nullptr)
				{
					createdDescription->m_quantize = ParseQuantizeType(quantizeAttribute);
				}

				const char *quantizeUnitAttribute = elem->Attribute("quantizeUnit");
				if (quantizeUnitAttribute != nullptr)
				{
					createdDescription->m_quantizeUnitExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(quantizeUnitAttribute, UIDescriptionValueType::kInteger));
				}

				return createdDescription;
			}

			//------------------------------------------------------------------------------
			template<>
			std::shared_ptr<ScaledVariable<FloatingPointValueType> > ScaledVariable<FloatingPointValueType>::ReadFromFromXMLElement(tinyxml2::XMLElement *elem)
			{
				const char *valueAttribute = elem->Attribute("value");
				if (valueAttribute == nullptr)
				{
					// No Value
					return nullptr;
				}

				std::shared_ptr<ScaledVariable<FloatingPointValueType> > createdDescription(std::make_shared<ScaledVariable<FloatingPointValueType> >());
				createdDescription->m_valueExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(valueAttribute, UIDescriptionValueType::kFloatingPoint));

				const char *scaleAttribute = elem->Attribute("scale");
				if (scaleAttribute != nullptr)
				{
					createdDescription->m_scaleExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(scaleAttribute, UIDescriptionValueType::kFloatingPoint));
				}

				const char *quantizeAttribute = elem->Attribute("quantize");
				if (quantizeAttribute != nullptr)
				{
					createdDescription->m_quantize = ParseQuantizeType(quantizeAttribute);
				}

				const char *quantizeUnitAttribute = elem->Attribute("quantizeUnit");
				if (quantizeUnitAttribute != nullptr)
				{
					createdDescription->m_quantizeUnitExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(quantizeUnitAttribute, UIDescriptionValueType::kFloatingPoint));
				}

				return createdDescription;
			}
		}
	}
}
