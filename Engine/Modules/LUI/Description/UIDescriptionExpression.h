////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionExpression.h (Leggiero/Modules - LegacyUI)
//
// Expression in UI Description
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_EXPRESSION_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_EXPRESSION_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <algorithm>
#include <exception>
#include <functional>
#include <memory>
#include <vector>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Expression
			{
				// Type independent expression interface
				class IArbitrarilyTypedExpression
					: public ITypeTagged
				{
				public:
					virtual ~IArbitrarilyTypedExpression() { }
				};


				// Interface for an Expression
				template <typename ValueT>
				class IExpression
					: public IArbitrarilyTypedExpression
				{
				public:
					virtual ~IExpression() { }

				public:	// ITypeTagged
					virtual UIDescriptionValueType GetValueType() const override { return ms_valueType; }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) = 0;

				public:
					static std::shared_ptr<IExpression<ValueT> > GetTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression);

				protected:
					const static UIDescriptionValueType ms_valueType;
				};


				// Decorator for an expression can cache evaluation result
				template <typename ValueT>
				class CachingExpression
					: public IExpression<ValueT>
				{
				public:
					CachingExpression(std::shared_ptr<IExpression<ValueT> > expression) : m_isEvaluated(false), m_expression(expression) { }
					virtual ~CachingExpression() { }

				public:
					static std::shared_ptr<CachingExpression<ValueT> > FromExpression(std::shared_ptr<IExpression<ValueT> > expression) { return std::make_shared<CachingExpression<ValueT> >(expression); }
					static std::shared_ptr<CachingExpression<ValueT> > FromArbitrarilyTypedExpression(std::shared_ptr<IArbitrarilyTypedExpression> arbitrarilyTypedExpression) { return std::make_shared<CachingExpression<ValueT> >(IExpression<ValueT>::GetTypedExpression(arbitrarilyTypedExpression)); }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						if (m_isEvaluated)
						{
							return m_cachedValue;
						}
						m_cachedValue = m_expression->Evaluate(processingContext);
						m_isEvaluated = true;
						return m_cachedValue;
					}

				protected:
					bool	m_isEvaluated;
					ValueT	m_cachedValue;

					std::shared_ptr<IExpression<ValueT> > m_expression;
				};


				//////////////////////////////////////////////////////////////////////////////// Expression Exceptions

				// Dereference not existing variable
				class UIDescriptionExpressionVariableNotExistException : public std::exception
				{
				public:
					UIDescriptionExpressionVariableNotExistException() { }
					virtual ~UIDescriptionExpressionVariableNotExistException() throw() { }

				public:
					virtual const char *what() const throw() { return "Variable Not Found"; }
				};

				// Type Error
				class UIDescriptionExpressionTypeErrorException : public std::exception
				{
				public:
					UIDescriptionExpressionTypeErrorException() { }
					virtual ~UIDescriptionExpressionTypeErrorException() throw() { }

				public:
					virtual const char *what() const throw() { return "Type Error"; }
				};

				// Out of Index
				class UIDescriptionExpressionOutOfIndexException : public std::exception
				{
				public:
					UIDescriptionExpressionOutOfIndexException() { }
					virtual ~UIDescriptionExpressionOutOfIndexException() throw() { }

				public:
					virtual const char *what() const throw() { return "Out of Index"; }
				};


				//////////////////////////////////////////////////////////////////////////////// Parse Functions

				UIDescriptionValueType ParseTypeFromString(const char *stringValue);
				UIDescriptionValueType DetectTypeFromString(const char *stringValue);

				BooleanValueType ParseBooleanValueFromString(const char *stringValue);
				IntegerValueType ParseIntegerValueFromString(const char *stringValue);
				FloatingPointValueType ParseFloatingPointValueFromString(const char *stringValue);
				ColorARGBValueType ParseColorValueFromString(const char *stringValue);
				Vector2DValueType ParseVector2DValueFromString(const char *stringValue);
				Vector3DValueType ParseVector3DValueFromString(const char *stringValue);
				StringValueType ParseStringValueFromString(const char *stringValue);


				namespace Reader
				{
					// Read inline-expression from the given string
					std::shared_ptr<IArbitrarilyTypedExpression> ParseInlineExpression(const char *expressionString, UIDescriptionValueType hintedType = UIDescriptionValueType::kINVALID);


					//////////////////////////////////////////////////////////////////////////////// Reader from XML Data

					namespace XML
					{
						// Read varaible contains an expression from given XML Element
						std::shared_ptr<IDescriptionObject> ReadExpressionVariableFromElement(ParsingXMLElementType *elem);

						// Read an expression from given XML Element
						std::shared_ptr<IArbitrarilyTypedExpression> ReadExpressionFromElement(ParsingXMLElementType *elem, UIDescriptionValueType hintedType = UIDescriptionValueType::kINVALID);
					}
				}
			}


			// Variable stores expression
			template <typename ValueT>
			class ExpressionVariable
				: public Variable<ValueT>
			{
			public:
				ExpressionVariable(std::shared_ptr<Expression::IExpression<ValueT> > exp) : m_expression(exp) { }
				virtual ~ExpressionVariable() { }

			protected:	// Variable<ValueT>
				virtual void _Evaluate(DescriptionProcessingContext &processingContext) override
				{
					this->m_value = m_expression->Evaluate(processingContext);
					this->m_isEvaluated = true;
				}

			protected:
				std::shared_ptr<Expression::IExpression<ValueT> > m_expression;
			};
		}
	}
}

#endif
