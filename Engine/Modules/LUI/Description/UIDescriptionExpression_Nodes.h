////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionExpression_Nodes.h (Leggiero/Modules - LegacyUI)
//
// Definition of calculation nodes in UI Description Expression
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_DESCRIPTION_EXPRESSION_NODES_H
#define __LM_LUI__DESCRIPTION__UI_DESCRIPTION_EXPRESSION_NODES_H


// Leggiero.LegacyUI
#include "UIDescriptionExpression.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Expression
			{
				// Constant
				template <typename ValueT>
				class Constant
					: public IExpression<ValueT>
				{
				public:
					Constant(ValueT constantValue) : m_value(constantValue) { }
					virtual ~Constant() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override { return m_value; }

				protected:
					ValueT	m_value;
				};


				// Variable Deferencing
				template <typename ValueT>
				class Dereference
					: public IExpression<ValueT>
				{
				public:
					Dereference(const VariablePathType &variablePath) : m_targetPath(variablePath) { }
					virtual ~Dereference() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					VariablePathType m_targetPath;
				};


				// Casting
				template <typename FromT, typename ToT>
				class Cast
					: public IExpression<ToT>
				{
				public:
					Cast(const std::shared_ptr<IExpression<FromT> > &sourceExpression) : m_toCast(sourceExpression) { }
					virtual ~Cast() { }

				public:
					virtual ToT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<FromT> > m_toCast;
				};


				// Unary Minus
				template <typename ValueT>
				class Minus
					: public IExpression<ValueT>
				{
				public:
					Minus(const std::shared_ptr<IExpression<ValueT> > &sourceExpression) : m_operand(sourceExpression) { }
					virtual ~Minus() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return -(m_operand->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand;
				};

				template<> BooleanValueType Minus<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext);


				// Addition
				template <typename ValueT>
				class Add
					: public IExpression<ValueT>
				{
				public:
					Add(const std::vector<std::shared_ptr<IExpression<ValueT> > > &sourceExpression) : m_operands(sourceExpression) { }
					Add(std::vector<std::shared_ptr<IExpression<ValueT> > > &&sourceExpression) : m_operands(sourceExpression) { }
					virtual ~Add() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT sum = m_operands[0]->Evaluate(processingContext);
						size_t operandsCount = m_operands.size();
						for (size_t i = 1; i < operandsCount; ++i)
						{
							sum += m_operands[i]->Evaluate(processingContext);
						}
						return sum;
					}

				protected:
					std::vector<std::shared_ptr<IExpression<ValueT> > > m_operands;
				};

				template<> BooleanValueType Add<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> ColorARGBValueType Add<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext);


				// Subtraction
				template <typename ValueT>
				class Subtract
					: public IExpression<ValueT>
				{
				public:
					Subtract(const std::shared_ptr<IExpression<ValueT> > &minuend, const std::shared_ptr<IExpression<ValueT> > &subtrahend) : m_minuend(minuend), m_subtrahend(subtrahend) { }
					virtual ~Subtract() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_minuend->Evaluate(processingContext) - m_subtrahend->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_minuend;
					std::shared_ptr<IExpression<ValueT> > m_subtrahend;
				};


				// Multiplication
				template <typename ValueT>
				class Multiply
					: public IExpression<ValueT>
				{
				public:
					Multiply(const std::vector<std::shared_ptr<IExpression<ValueT> > > &sourceExpression) : m_operands(sourceExpression) { }
					Multiply(std::vector<std::shared_ptr<IExpression<ValueT> > > &&sourceExpression) : m_operands(sourceExpression) { }
					virtual ~Multiply() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT product = m_operands[0]->Evaluate(processingContext);
						size_t operandsCount = m_operands.size();
						for (size_t i = 1; i < operandsCount; ++i)
						{
							product *= m_operands[i]->Evaluate(processingContext);
						}
						return product;
					}

				protected:
					std::vector<std::shared_ptr<IExpression<ValueT> > > m_operands;
				};

				template<> BooleanValueType Multiply<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> ColorARGBValueType Multiply<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext);

				template<> Vector2DValueType Multiply<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> Vector3DValueType Multiply<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> StringValueType Multiply<StringValueType>::Evaluate(DescriptionProcessingContext &processingContext);


				// Division
				template <typename ValueT>
				class Divide
					: public IExpression<ValueT>
				{
				public:
					Divide(const std::shared_ptr<IExpression<ValueT> > &dividend, const std::shared_ptr<IExpression<ValueT> > &divisor) : m_dividend(dividend), m_divisor(divisor) { }
					virtual ~Divide() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_dividend->Evaluate(processingContext) / m_divisor->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_dividend;
					std::shared_ptr<IExpression<ValueT> > m_divisor;
				};


				// Integer Modulus
				class Modulus
					: public IExpression<IntegerValueType>
				{
				public:
					Modulus(const std::shared_ptr<IExpression<IntegerValueType> > &dividend, const std::shared_ptr<IExpression<IntegerValueType> > &divisor) : m_dividend(dividend), m_divisor(divisor) { }
					virtual ~Modulus() { }

				public:
					virtual IntegerValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_dividend->Evaluate(processingContext) % m_divisor->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<IntegerValueType> > m_dividend;
					std::shared_ptr<IExpression<IntegerValueType> > m_divisor;
				};


				// Floating Point Modulus
				class Remainder
					: public IExpression<FloatingPointValueType>
				{
				public:
					Remainder(const std::shared_ptr<IExpression<FloatingPointValueType> > &dividend, const std::shared_ptr<IExpression<FloatingPointValueType> > &divisor) : m_dividend(dividend), m_divisor(divisor) { }
					virtual ~Remainder() { }

				public:
					virtual FloatingPointValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_dividend;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_divisor;
				};


				// Linear Interpolation
				template <typename ValueT>
				class Lerp
					: public IExpression<ValueT>
				{
				public:
					Lerp(const std::shared_ptr<IExpression<FloatingPointValueType> > &lerpParameter, const std::shared_ptr<IExpression<ValueT> > &value1, const std::shared_ptr<IExpression<ValueT> > &value2) : m_parameter(lerpParameter), m_value1(value1), m_value2(value2) { }
					virtual ~Lerp() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_parameter;
					std::shared_ptr<IExpression<ValueT> > m_value1;
					std::shared_ptr<IExpression<ValueT> > m_value2;
				};


				// Logical Not
				class Not
					: public IExpression<BooleanValueType>
				{
				public:
					Not(const std::shared_ptr<IExpression<BooleanValueType> > &operand) : m_operand(operand) { }
					virtual ~Not() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return !(m_operand->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<BooleanValueType> > m_operand;
				};


				// Logical And
				class And
					: public IExpression<BooleanValueType>
				{
				public:
					And(const std::shared_ptr<IExpression<BooleanValueType> > &operand1, const std::shared_ptr<IExpression<BooleanValueType> > &operand2) : m_operand1(operand1), m_operand2(operand2) { }
					virtual ~And() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand1->Evaluate(processingContext) && m_operand2->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<BooleanValueType> > m_operand1;
					std::shared_ptr<IExpression<BooleanValueType> > m_operand2;
				};


				// Logical Or
				class Or
					: public IExpression<BooleanValueType>
				{
				public:
					Or(const std::shared_ptr<IExpression<BooleanValueType> > &operand1, const std::shared_ptr<IExpression<BooleanValueType> > &operand2) : m_operand1(operand1), m_operand2(operand2) { }
					virtual ~Or() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand1->Evaluate(processingContext) || m_operand2->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<BooleanValueType> > m_operand1;
					std::shared_ptr<IExpression<BooleanValueType> > m_operand2;
				};


				// Logical Xor
				class Xor
					: public IExpression<BooleanValueType>
				{
				public:
					Xor(const std::shared_ptr<IExpression<BooleanValueType> > &operand1, const std::shared_ptr<IExpression<BooleanValueType> > &operand2) : m_operand1(operand1), m_operand2(operand2) { }
					virtual ~Xor() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand1->Evaluate(processingContext) != m_operand2->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<BooleanValueType> > m_operand1;
					std::shared_ptr<IExpression<BooleanValueType> > m_operand2;
				};


				// Equality Check
				template <typename ValueT>
				class IsEqual
					: public IExpression<BooleanValueType>
				{
				public:
					IsEqual(const std::shared_ptr<IExpression<ValueT> > &operand1, const std::shared_ptr<IExpression<ValueT> > &operand2) : m_operand1(operand1), m_operand2(operand2) { }
					virtual ~IsEqual() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand1->Evaluate(processingContext) == m_operand2->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand1;
					std::shared_ptr<IExpression<ValueT> > m_operand2;
				};


				// Compare for not equal
				template <typename ValueT>
				class IsNotEqual
					: public IExpression<BooleanValueType>
				{
				public:
					IsNotEqual(const std::shared_ptr<IExpression<ValueT> > &operand1, const std::shared_ptr<IExpression<ValueT> > &operand2) : m_operand1(operand1), m_operand2(operand2) { }
					virtual ~IsNotEqual() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand1->Evaluate(processingContext) != m_operand2->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand1;
					std::shared_ptr<IExpression<ValueT> > m_operand2;
				};


				// Order Check
				template <typename ValueT>
				class IsGreater
					: public IExpression<BooleanValueType>
				{
				public:
					IsGreater(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &compareTo) : m_operand(operand), m_compareTo(compareTo) { }
					virtual ~IsGreater() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand->Evaluate(processingContext) > m_compareTo->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand;
					std::shared_ptr<IExpression<ValueT> > m_compareTo;
				};

				template<> BooleanValueType IsGreater<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsGreater<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);

				template <typename ValueT>
				class IsLess
					: public IExpression<BooleanValueType>
				{
				public:
					IsLess(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &compareTo) : m_operand(operand), m_compareTo(compareTo) { }
					virtual ~IsLess() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand->Evaluate(processingContext) < m_compareTo->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand;
					std::shared_ptr<IExpression<ValueT> > m_compareTo;
				};

				template<> BooleanValueType IsLess<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsLess<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);

				template <typename ValueT>
				class IsGreaterEqual
					: public IExpression<BooleanValueType>
				{
				public:
					IsGreaterEqual(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &compareTo) : m_operand(operand), m_compareTo(compareTo) { }
					virtual ~IsGreaterEqual() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand->Evaluate(processingContext) >= m_compareTo->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand;
					std::shared_ptr<IExpression<ValueT> > m_compareTo;
				};

				template<> BooleanValueType IsGreaterEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsGreaterEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);

				template <typename ValueT>
				class IsLessEqual
					: public IExpression<BooleanValueType>
				{
				public:
					IsLessEqual(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &compareTo) : m_operand(operand), m_compareTo(compareTo) { }
					virtual ~IsLessEqual() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return (m_operand->Evaluate(processingContext) <= m_compareTo->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_operand;
					std::shared_ptr<IExpression<ValueT> > m_compareTo;
				};

				template<> BooleanValueType IsLessEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsLessEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);


				// Approximate Compare
				template <typename ValueT>
				class IsApproxEqual
					: public IExpression<BooleanValueType>
				{
				public:
					IsApproxEqual(const std::shared_ptr<IExpression<ValueT> > &errorLimit, const std::shared_ptr<IExpression<ValueT> > &operand1, const std::shared_ptr<IExpression<ValueT> > &operand2) : m_errorLimit(errorLimit), m_operand1(operand1), m_operand2(operand2) { }
					virtual ~IsApproxEqual() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<ValueT> > m_errorLimit;
					std::shared_ptr<IExpression<ValueT> > m_operand1;
					std::shared_ptr<IExpression<ValueT> > m_operand2;
				};

				template<> BooleanValueType IsApproxEqual<FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsApproxEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsApproxEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);

				template <typename ValueT>
				class IsApproxFloatEqual
					: public IExpression<BooleanValueType>
				{
				public:
					IsApproxFloatEqual(const std::shared_ptr<IExpression<FloatingPointValueType> > &errorLimit, const std::shared_ptr<IExpression<ValueT> > &operand1, const std::shared_ptr<IExpression<ValueT> > &operand2) : m_errorLimit(errorLimit), m_operand1(operand1), m_operand2(operand2) { }
					virtual ~IsApproxFloatEqual() { }

				public:
					virtual BooleanValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_errorLimit;
					std::shared_ptr<IExpression<ValueT> > m_operand1;
					std::shared_ptr<IExpression<ValueT> > m_operand2;
				};

				template<> BooleanValueType IsApproxFloatEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> BooleanValueType IsApproxFloatEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext);


				// Conditional Value
				template <typename ValueT>
				class Iff
					: public IExpression<ValueT>
				{
				public:
					Iff(const std::shared_ptr<IExpression<BooleanValueType> > &condition, const std::shared_ptr<IExpression<ValueT> > &value1, const std::shared_ptr<IExpression<ValueT> > &value2) : m_condition(condition), m_value1(value1), m_value2(value2) { }
					virtual ~Iff() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						if (m_condition->Evaluate(processingContext))
						{
							return m_value1->Evaluate(processingContext);
						}
						else
						{
							return m_value2->Evaluate(processingContext);
						}
					}

				protected:
					std::shared_ptr<IExpression<BooleanValueType> > m_condition;
					std::shared_ptr<IExpression<ValueT> > m_value1;
					std::shared_ptr<IExpression<ValueT> > m_value2;
				};


				// Find Minimum Value
				template <typename ValueT>
				class Min
					: public IExpression<ValueT>
				{
				public:
					Min(const std::vector<std::shared_ptr<IExpression<ValueT> > > &sourceExpression) : m_operands(sourceExpression) { }
					Min(std::vector<std::shared_ptr<IExpression<ValueT> > > &&sourceExpression) : m_operands(sourceExpression) { }
					virtual ~Min() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT currentMin = m_operands[0]->Evaluate(processingContext);
						size_t operandsCount = m_operands.size();
						for (size_t i = 1; i < operandsCount; ++i)
						{
							ValueT currentValue = m_operands[i]->Evaluate(processingContext);
							if (_IsSmaller(currentValue, currentMin))
							{
								currentMin = currentValue;
							}
						}
						return currentMin;
					}

				protected:
					bool _IsSmaller(const ValueT &a, const ValueT &b) const
					{
						return (a < b);
					}

				protected:
					std::vector<std::shared_ptr<IExpression<ValueT> > > m_operands;
				};

				template<> bool Min<BooleanValueType>::_IsSmaller(const BooleanValueType &a, const BooleanValueType &b) const;
				template<> bool Min<ColorARGBValueType>::_IsSmaller(const ColorARGBValueType &a, const ColorARGBValueType &b) const;
				template<> bool Min<Vector2DValueType>::_IsSmaller(const Vector2DValueType &a, const Vector2DValueType &b) const;
				template<> bool Min<Vector3DValueType>::_IsSmaller(const Vector3DValueType &a, const Vector3DValueType &b) const;

				template<> BooleanValueType Min<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> ColorARGBValueType Min<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext);

				// Find Maximum Value
				template <typename ValueT>
				class Max
					: public IExpression<ValueT>
				{
				public:
					Max(const std::vector<std::shared_ptr<IExpression<ValueT> > > &sourceExpression) : m_operands(sourceExpression) { }
					Max(std::vector<std::shared_ptr<IExpression<ValueT> > > &&sourceExpression) : m_operands(sourceExpression) { }
					virtual ~Max() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT currentMax = m_operands[0]->Evaluate(processingContext);
						size_t operandsCount = m_operands.size();
						for (size_t i = 1; i < operandsCount; ++i)
						{
							ValueT currentValue = m_operands[i]->Evaluate(processingContext);
							if (_IsBigger(currentValue, currentMax))
							{
								currentMax = currentValue;
							}
						}
						return currentMax;
					}

				protected:
					bool _IsBigger(const ValueT &a, const ValueT &b) const
					{
						return (a < b);
					}

				protected:
					std::vector<std::shared_ptr<IExpression<ValueT> > > m_operands;
				};

				template<> bool Max<BooleanValueType>::_IsBigger(const BooleanValueType &a, const BooleanValueType &b) const;
				template<> bool Max<ColorARGBValueType>::_IsBigger(const ColorARGBValueType &a, const ColorARGBValueType &b) const;
				template<> bool Max<Vector2DValueType>::_IsBigger(const Vector2DValueType &a, const Vector2DValueType &b) const;
				template<> bool Max<Vector3DValueType>::_IsBigger(const Vector3DValueType &a, const Vector3DValueType &b) const;

				template<> BooleanValueType Max<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext);
				template<> ColorARGBValueType Max<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext);


				// Absolute Value
				template <typename SourceValueT, typename ResultValueT>
				class Abs
					: public IExpression<ResultValueT>
				{
				public:
					Abs(const std::shared_ptr<IExpression<SourceValueT> > &sourceExpression) : m_operand(sourceExpression) { }
					virtual ~Abs() { }

				public:
					virtual ResultValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<SourceValueT> > m_operand;
				};


				// Power
				template <typename ValueT>
				class Power
					: public IExpression<FloatingPointValueType>
				{
				public:
					Power(const std::shared_ptr<IExpression<ValueT> > &baseExpression, const std::shared_ptr<IExpression<ValueT> > &exponentExpression) : m_base(baseExpression), m_exponent(exponentExpression) { }
					virtual ~Power() { }

				public:
					virtual FloatingPointValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return _CalculatePower(m_base->Evaluate(processingContext), m_exponent->Evaluate(processingContext));
					}

				protected:
					FloatingPointValueType _CalculatePower(const ValueT &base, const ValueT &exponent) const;

				protected:
					std::shared_ptr<IExpression<ValueT> > m_base;
					std::shared_ptr<IExpression<ValueT> > m_exponent;
				};


				// 1-argument floating point function
				class AuxFloatingPointFunction
					: public IExpression<FloatingPointValueType>
				{
				public:
					AuxFloatingPointFunction(std::function<FloatingPointValueType(FloatingPointValueType)> applyingFunction, const std::shared_ptr<IExpression<FloatingPointValueType> > &operand) : m_func(applyingFunction), m_operand(operand) { }
					virtual ~AuxFloatingPointFunction() { }

				public:
					virtual FloatingPointValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return m_func(m_operand->Evaluate(processingContext));
					}

				protected:
					std::function<FloatingPointValueType(FloatingPointValueType)>	m_func;
					std::shared_ptr<IExpression<FloatingPointValueType> >			m_operand;
				};


				// To Integer Functions
				class Integerization
					: public IExpression<IntegerValueType>
				{
				public:
					Integerization(const std::shared_ptr<IExpression<FloatingPointValueType> > &operand) : m_operand(operand) { }
					virtual ~Integerization() { }

				public:
					virtual IntegerValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return _Integerize(m_operand->Evaluate(processingContext));
					}

				protected:
					virtual IntegerValueType _Integerize(FloatingPointValueType sourceValue) const = 0;

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_operand;
				};

				class Round
					: public Integerization
				{
				public:
					Round(const std::shared_ptr<IExpression<FloatingPointValueType> > &operand) : Integerization(operand) { }
					virtual ~Round() { }

				protected:
					virtual IntegerValueType _Integerize(FloatingPointValueType sourceValue) const override;
				};

				class Floor
					: public Integerization
				{
				public:
					Floor(const std::shared_ptr<IExpression<FloatingPointValueType> > &operand) : Integerization(operand) { }
					virtual ~Floor() { }

				protected:
					virtual IntegerValueType _Integerize(FloatingPointValueType sourceValue) const override;
				};

				class Ceil
					: public Integerization
				{
				public:
					Ceil(const std::shared_ptr<IExpression<FloatingPointValueType> > &operand) : Integerization(operand) { }
					virtual ~Ceil() { }

				protected:
					virtual IntegerValueType _Integerize(FloatingPointValueType sourceValue) const override;
				};


				// Clamp Function
				template <typename ValueT>
				class Clamp
					: public IExpression<ValueT>
				{
				public:
					Clamp(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &low, const std::shared_ptr<IExpression<ValueT> > &high) : m_operand(operand), m_low(low), m_high(high) { }
					virtual ~Clamp() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT resultValue = m_operand->Evaluate(processingContext);
						ValueT lowClampValue = m_low->Evaluate(processingContext);
						ValueT highClampValue = m_high->Evaluate(processingContext);
						if (resultValue < lowClampValue)
						{
							return lowClampValue;
						}
						else if (resultValue > highClampValue)
						{
							return highClampValue;
						}
						return resultValue;
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_low;
					std::shared_ptr<IExpression<ValueT> > m_high;
					std::shared_ptr<IExpression<ValueT> > m_operand;
				};

				template <typename ValueT>
				class ClampLow
					: public IExpression<ValueT>
				{
				public:
					ClampLow(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &low) : m_operand(operand), m_low(low) { }
					virtual ~ClampLow() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT resultValue = m_operand->Evaluate(processingContext);
						ValueT lowClampValue = m_low->Evaluate(processingContext);
						if (resultValue < lowClampValue)
						{
							return lowClampValue;
						}
						return resultValue;
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_low;
					std::shared_ptr<IExpression<ValueT> > m_operand;
				};

				template <typename ValueT>
				class ClampHigh
					: public IExpression<ValueT>
				{
				public:
					ClampHigh(const std::shared_ptr<IExpression<ValueT> > &operand, const std::shared_ptr<IExpression<ValueT> > &high) : m_operand(operand), m_high(high) { }
					virtual ~ClampHigh() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override
					{
						ValueT resultValue = m_operand->Evaluate(processingContext);
						ValueT highClampValue = m_high->Evaluate(processingContext);
						if (resultValue > highClampValue)
						{
							return highClampValue;
						}
						return resultValue;
					}

				protected:
					std::shared_ptr<IExpression<ValueT> > m_high;
					std::shared_ptr<IExpression<ValueT> > m_operand;
				};


				// Texture Property Extraction
				template <typename ValueT>
				class TextureGetWidth
					: public IExpression<ValueT>
				{
				public:
					TextureGetWidth(const VariablePathType &texturePath) : m_targetTexturePath(texturePath) { }
					virtual ~TextureGetWidth() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					VariablePathType m_targetTexturePath;
				};

				template <typename ValueT>
				class TextureGetHeight
					: public IExpression<ValueT>
				{
				public:
					TextureGetHeight(const VariablePathType &texturePath) : m_targetTexturePath(texturePath) { }
					virtual ~TextureGetHeight() { }

				public:
					virtual ValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					VariablePathType m_targetTexturePath;
				};


				// Color
				class ConstructColorARGB
					: public IExpression<ColorARGBValueType>
				{
				public:
					ConstructColorARGB(const std::shared_ptr<IExpression<IntegerValueType> > &alphaExpression, const std::shared_ptr<IExpression<IntegerValueType> > &redExpression, const std::shared_ptr<IExpression<IntegerValueType> > &greenExpression, const std::shared_ptr<IExpression<IntegerValueType> > &blueExpression)
						: m_alpha(alphaExpression), m_red(redExpression), m_green(greenExpression), m_blue(blueExpression)
					{ }
					virtual ~ConstructColorARGB() { }

				public:
					virtual ColorARGBValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return ColorARGBValueType(m_alpha->Evaluate(processingContext), m_red->Evaluate(processingContext), m_green->Evaluate(processingContext), m_blue->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<IntegerValueType> > m_alpha;
					std::shared_ptr<IExpression<IntegerValueType> > m_red;
					std::shared_ptr<IExpression<IntegerValueType> > m_green;
					std::shared_ptr<IExpression<IntegerValueType> > m_blue;
				};

				class ConstructColorAHSL
					: public IExpression<ColorARGBValueType>
				{
				public:
					ConstructColorAHSL(bool isDegreeHue, const std::shared_ptr<IExpression<FloatingPointValueType> > &alphaExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &hueExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &saturationExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &lightnessExpression)
						: m_alpha(alphaExpression), m_hue(hueExpression), m_saturation(saturationExpression), m_ligntness(lightnessExpression), m_isDegreeHue(isDegreeHue)
					{ }
					virtual ~ConstructColorAHSL() { }

				public:
					virtual ColorARGBValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						FloatingPointValueType hueValue = m_hue->Evaluate(processingContext);
						if (m_isDegreeHue)
						{
							hueValue /= static_cast<FloatingPointValueType>(360.0f);
						}
						Graphics::GLByteRGB colorValue = Graphics::HSLColor(hueValue, m_saturation->Evaluate(processingContext), m_ligntness->Evaluate(processingContext)).ToRGB();

						FloatingPointValueType alphaFloat = m_alpha->Evaluate(processingContext) * 255.0f;
						GLubyte alpha = ((alphaFloat < 0.0f) ? 0 : ((alphaFloat > 255.0f) ? 255 : static_cast<GLubyte>(alphaFloat)));
						return ColorARGBValueType(alpha, colorValue.red, colorValue.green, colorValue.blue);
					}

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_alpha;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_hue;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_saturation;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_ligntness;

					bool m_isDegreeHue;
				};

				// Vector Construct
				class ConstructVector2D
					: public IExpression<Vector2DValueType>
				{
				public:
					ConstructVector2D(const std::shared_ptr<IExpression<FloatingPointValueType> > &xExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &yExpression) : m_x(xExpression), m_y(yExpression) { }
					virtual ~ConstructVector2D() { }

				public:
					virtual Vector2DValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return Vector2DValueType(m_x->Evaluate(processingContext), m_y->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_x;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_y;
				};

				class ConstructVector3D
					: public IExpression<Vector3DValueType>
				{
				public:
					ConstructVector3D(const std::shared_ptr<IExpression<FloatingPointValueType> > &xExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &yExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &zExpression) : m_x(xExpression), m_y(yExpression), m_z(zExpression) { }
					virtual ~ConstructVector3D() { }

				public:
					virtual Vector3DValueType Evaluate(DescriptionProcessingContext &processingContext) override
					{
						return Vector3DValueType(m_x->Evaluate(processingContext), m_y->Evaluate(processingContext), m_y->Evaluate(processingContext));
					}

				protected:
					std::shared_ptr<IExpression<FloatingPointValueType> > m_x;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_y;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_z;
				};

				// Extract Component
				template <typename SourceValueT, typename ResultValueT>
				class ComponentOf
					: public IExpression<ResultValueT>
				{
				public:
					ComponentOf(const std::shared_ptr<IExpression<SourceValueT> > &sourceExpression, const std::shared_ptr<IExpression<IntegerValueType> > &indexExpression) : m_operand(sourceExpression), m_index(indexExpression) { }
					virtual ~ComponentOf() { }

				public:
					virtual ResultValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<SourceValueT> > m_operand;
					std::shared_ptr<IExpression<IntegerValueType> > m_index;
				};

				// Vector Operations
				class VectorDot
					: public IExpression<FloatingPointValueType>
				{
				public:
					VectorDot(const std::shared_ptr<IExpression<Vector3DValueType> > &vector1Expression, const std::shared_ptr<IExpression<Vector3DValueType> > &vector2Expression) : m_vector1(vector1Expression), m_vector2(vector2Expression) { }
					virtual ~VectorDot() { }

				public:
					virtual FloatingPointValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<Vector3DValueType> > m_vector1;
					std::shared_ptr<IExpression<Vector3DValueType> > m_vector2;
				};

				class VectorCross
					: public IExpression<Vector3DValueType>
				{
				public:
					VectorCross(const std::shared_ptr<IExpression<Vector3DValueType> > &vector1Expression, const std::shared_ptr<IExpression<Vector3DValueType> > &vector2Expression) : m_vector1(vector1Expression), m_vector2(vector2Expression) { }
					virtual ~VectorCross() { }

				public:
					virtual Vector3DValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<Vector3DValueType> > m_vector1;
					std::shared_ptr<IExpression<Vector3DValueType> > m_vector2;
				};

				template <typename SourceValueT>
				class VectorScalarMultiply
					: public IExpression<SourceValueT>
				{
				public:
					VectorScalarMultiply(const std::shared_ptr<IExpression<SourceValueT> > &vectorExpression, const std::shared_ptr<IExpression<FloatingPointValueType> > &multiplierExpression) : m_vector(vectorExpression), m_multiplier(multiplierExpression) { }
					virtual ~VectorScalarMultiply() { }

				public:
					virtual SourceValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<SourceValueT> > m_vector;
					std::shared_ptr<IExpression<FloatingPointValueType> > m_multiplier;
				};

				template <typename SourceValueT>
				class VectorNormal
					: public IExpression<SourceValueT>
				{
				public:
					VectorNormal(const std::shared_ptr<IExpression<SourceValueT> > &vectorExpression) : m_vector(vectorExpression) { }
					virtual ~VectorNormal() { }

				public:
					virtual SourceValueT Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<SourceValueT> > m_vector;
				};

				template <typename SourceValueT>
				class VectorLength
					: public IExpression<FloatingPointValueType>
				{
				public:
					VectorLength(const std::shared_ptr<IExpression<SourceValueT> > &vectorExpression) : m_vector(vectorExpression) { }
					virtual ~VectorLength() { }

				public:
					virtual FloatingPointValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<SourceValueT> > m_vector;
				};

				template <typename SourceValueT>
				class VectorSquareLength
					: public IExpression<FloatingPointValueType>
				{
				public:
					VectorSquareLength(const std::shared_ptr<IExpression<SourceValueT> > &vectorExpression) : m_vector(vectorExpression) { }
					virtual ~VectorSquareLength() { }

				public:
					virtual FloatingPointValueType Evaluate(DescriptionProcessingContext &processingContext) override;

				protected:
					std::shared_ptr<IExpression<SourceValueT> > m_vector;
				};
			}
		}
	}
}

#endif
