////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionExpression_NodesEvaluation.inl (Leggiero/Modules - LegacyUI)
//
// Evaluation Implementation of Expression Nodes
// Included in UIDescriptionExpression.cpp
////////////////////////////////////////////////////////////////////////////////


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Expression
			{
				//------------------------------------------------------------------------------ Dereference
				template<>
				BooleanValueType Dereference<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<BooleanValueType>(processingContext, m_targetPath);
				}

				template<>
				IntegerValueType Dereference<IntegerValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<IntegerValueType>(processingContext, m_targetPath);
				}

				template<>
				FloatingPointValueType Dereference<FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<FloatingPointValueType>(processingContext, m_targetPath);
				}

				template<>
				ColorARGBValueType Dereference<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<ColorARGBValueType>(processingContext, m_targetPath);
				}

				template<>
				Vector2DValueType Dereference<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<Vector2DValueType>(processingContext, m_targetPath);
				}

				template<>
				Vector3DValueType Dereference<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<Vector3DValueType>(processingContext, m_targetPath);
				}

				template<>
				StringValueType Dereference<StringValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return _Internal::GetDereferencedValue<StringValueType>(processingContext, m_targetPath);
				}

				//------------------------------------------------------------------------------ Evaluate
				#define _MAKE_CAST_EVAL_FROM_TEMPLATE(FromT, ToT) template<> \
				ToT Cast<FromT, ToT>::Evaluate(DescriptionProcessingContext &processingContext) \
				{ \
					return _Internal::CastValue<FromT, ToT>(m_toCast->Evaluate(processingContext)); \
				}

				// Specialization for each types are in a separated file
				#include "UIDescriptionExpression_CastEvaluation.inl"

				//------------------------------------------------------------------------------ Minus
				template<>
				BooleanValueType Minus<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return !(m_operand->Evaluate(processingContext));
				}

				//------------------------------------------------------------------------------ Add
				template<>
				BooleanValueType Add<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					BooleanValueType sum = m_operands[0]->Evaluate(processingContext);
					size_t operandsCount = m_operands.size();
					for (size_t i = 1; i < operandsCount && !sum; ++i)
					{
						sum |= m_operands[i]->Evaluate(processingContext);
					}
					return sum;
				}

				template<>
				ColorARGBValueType Add<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					ColorARGBValueType evaluted = m_operands[0]->Evaluate(processingContext);
					int aSum = evaluted.alpha;
					int rSum = evaluted.red;
					int gSum = evaluted.green;
					int bSum = evaluted.blue;
					size_t operandsCount = m_operands.size();
					for (size_t i = 1; i < operandsCount; ++i)
					{
						evaluted = m_operands[i]->Evaluate(processingContext);
						aSum += evaluted.alpha;
						rSum += evaluted.red;
						gSum += evaluted.green;
						bSum += evaluted.blue;
					}
					return ColorARGBValueType(aSum, rSum, gSum, bSum);
				}

				//------------------------------------------------------------------------------ Multiply
				template<>
				BooleanValueType Multiply<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					BooleanValueType product = m_operands[0]->Evaluate(processingContext);
					size_t operandsCount = m_operands.size();
					for (size_t i = 1; i < operandsCount && product; ++i)
					{
						product &= m_operands[i]->Evaluate(processingContext);
					}
					return product;
				}

				template<>
				ColorARGBValueType Multiply<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					ColorARGBValueType evaluted = m_operands[0]->Evaluate(processingContext);
					float aProduct = (float)evaluted.alpha / 255.0f;
					float rProduct = (float)evaluted.red / 255.0f;
					float gProduct = (float)evaluted.green / 255.0f;
					float bProduct = (float)evaluted.blue / 255.0f;
					size_t operandsCount = m_operands.size();
					for (size_t i = 1; i < operandsCount; ++i)
					{
						evaluted = m_operands[i]->Evaluate(processingContext);
						aProduct *= (float)evaluted.alpha / 255.0f;
						rProduct *= (float)evaluted.red / 255.0f;
						gProduct *= (float)evaluted.green / 255.0f;
						bProduct *= (float)evaluted.blue / 255.0f;
					}
					return ColorARGBValueType(aProduct, rProduct, gProduct, bProduct);
				}

				template<>
				Vector2DValueType Multiply<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				Vector3DValueType Multiply<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				StringValueType Multiply<StringValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				//------------------------------------------------------------------------------ Remainder
				FloatingPointValueType Remainder::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return static_cast<FloatingPointValueType>(fmod(m_dividend->Evaluate(processingContext), m_divisor->Evaluate(processingContext)));
				}

				//------------------------------------------------------------------------------ Lerp
				template<>
				BooleanValueType Lerp<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType parameter = m_parameter->Evaluate(processingContext);
					if (parameter >= static_cast<FloatingPointValueType>(0.5f))
					{
						return m_value2->Evaluate(processingContext);
					}
					else
					{
						return m_value1->Evaluate(processingContext);
					}
				}

				template<>
				IntegerValueType Lerp<IntegerValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType parameter = m_parameter->Evaluate(processingContext);
					FloatingPointValueType inverseParameter = static_cast<FloatingPointValueType>(1.0f) - parameter;
					IntegerValueType evaluatedValue1 = m_value1->Evaluate(processingContext);
					IntegerValueType evaluatedValue2 = m_value2->Evaluate(processingContext);
					return static_cast<IntegerValueType>(static_cast<FloatingPointValueType>(evaluatedValue1) * inverseParameter + static_cast<FloatingPointValueType>(evaluatedValue2) * parameter);
				}

				template<>
				FloatingPointValueType Lerp<FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType parameter = m_parameter->Evaluate(processingContext);
					FloatingPointValueType inverseParameter = static_cast<FloatingPointValueType>(1.0f) - parameter;
					FloatingPointValueType evaluatedValue1 = m_value1->Evaluate(processingContext);
					FloatingPointValueType evaluatedValue2 = m_value2->Evaluate(processingContext);
					return (evaluatedValue1 * inverseParameter + evaluatedValue2 * parameter);
				}

				template<>
				ColorARGBValueType Lerp<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType parameter = m_parameter->Evaluate(processingContext);
					FloatingPointValueType inverseParameter = static_cast<FloatingPointValueType>(1.0f) - parameter;
					ColorARGBValueType evaluatedValue1 = m_value1->Evaluate(processingContext);
					ColorARGBValueType evaluatedValue2 = m_value2->Evaluate(processingContext);

					Graphics::HSLColor hslColor1 = evaluatedValue1.ToGLByteRGB().ToHSL();
					Graphics::HSLColor hslColor2 = evaluatedValue2.ToGLByteRGB().ToHSL();

					Graphics::HSLColor mixedHSLColor(
						(float)(hslColor1.hue * (float)inverseParameter + hslColor2.hue * (float)parameter),
						(float)(hslColor1.saturation * (float)inverseParameter + hslColor2.saturation * (float)parameter),
						(float)(hslColor1.lightness * (float)inverseParameter + hslColor2.lightness * (float)parameter)
					);
					Graphics::GLByteRGB mixedRGBColor(mixedHSLColor.ToRGB());

					GLubyte mixedAlpha = (GLubyte)((float)evaluatedValue1.alpha * inverseParameter + (float)evaluatedValue2.alpha * parameter);

					return ColorARGBValueType(mixedAlpha, mixedRGBColor.red, mixedRGBColor.green, mixedRGBColor.blue);
				}

				template<>
				Vector2DValueType Lerp<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType parameter = m_parameter->Evaluate(processingContext);
					return m_value1->Evaluate(processingContext).LerpWith(m_value2->Evaluate(processingContext), parameter);
				}

				template<>
				Vector3DValueType Lerp<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType parameter = m_parameter->Evaluate(processingContext);
					return m_value1->Evaluate(processingContext).LerpWith(m_value2->Evaluate(processingContext), parameter);
				}

				//------------------------------------------------------------------------------ Compare Operators
				template<>
				BooleanValueType IsGreater<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() > m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsGreater<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() > m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsLess<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() < m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsLess<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() < m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsGreaterEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() >= m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsGreaterEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() >= m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsLessEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() <= m_compareTo->Evaluate(processingContext).SquareLength());
				}

				template<>
				BooleanValueType IsLessEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (m_operand->Evaluate(processingContext).SquareLength() <= m_compareTo->Evaluate(processingContext).SquareLength());
				}

				//------------------------------------------------------------------------------ Approximate Equiv. Compare Operators
				template<>
				BooleanValueType IsApproxEqual<FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return (std::abs(m_operand1->Evaluate(processingContext) - m_operand2->Evaluate(processingContext)) < m_errorLimit->Evaluate(processingContext));
				}

				template<>
				BooleanValueType IsApproxEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					Vector2DValueType errorLimit = m_errorLimit->Evaluate(processingContext);
					Vector2DValueType evaluatedValue1 = m_operand1->Evaluate(processingContext);
					Vector2DValueType evaluatedValue2 = m_operand2->Evaluate(processingContext);
					return ((std::abs(evaluatedValue1.x - evaluatedValue2.x) < errorLimit.x)
						&& (std::abs(evaluatedValue1.y - evaluatedValue2.y) < errorLimit.y));
				}

				template<>
				BooleanValueType IsApproxEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					Vector3DValueType errorLimit = m_errorLimit->Evaluate(processingContext);
					Vector3DValueType evaluatedValue1 = m_operand1->Evaluate(processingContext);
					Vector3DValueType evaluatedValue2 = m_operand2->Evaluate(processingContext);
					return ((std::abs(evaluatedValue1.x - evaluatedValue2.x) < errorLimit.x)
						&& (std::abs(evaluatedValue1.y - evaluatedValue2.y) < errorLimit.y)
						&& (std::abs(evaluatedValue1.z - evaluatedValue2.z) < errorLimit.z));
				}

				template<>
				BooleanValueType IsApproxFloatEqual<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType errorLimit = m_errorLimit->Evaluate(processingContext);
					Vector2DValueType evaluatedValue1 = m_operand1->Evaluate(processingContext);
					Vector2DValueType evaluatedValue2 = m_operand2->Evaluate(processingContext);
					return ((std::abs(evaluatedValue1.x - evaluatedValue2.x) < errorLimit)
						&& (std::abs(evaluatedValue1.y - evaluatedValue2.y) < errorLimit));
				}

				template<>
				BooleanValueType IsApproxFloatEqual<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					FloatingPointValueType errorLimit = m_errorLimit->Evaluate(processingContext);
					Vector3DValueType evaluatedValue1 = m_operand1->Evaluate(processingContext);
					Vector3DValueType evaluatedValue2 = m_operand2->Evaluate(processingContext);
					return ((std::abs(evaluatedValue1.x - evaluatedValue2.x) < errorLimit)
						&& (std::abs(evaluatedValue1.y - evaluatedValue2.y) < errorLimit)
						&& (std::abs(evaluatedValue1.z - evaluatedValue2.z) < errorLimit));
				}

				//------------------------------------------------------------------------------ Select Operators
				template<>
				bool Min<BooleanValueType>::_IsSmaller(const BooleanValueType &a, const BooleanValueType &b) const
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				bool Min<ColorARGBValueType>::_IsSmaller(const ColorARGBValueType &a, const ColorARGBValueType &b) const
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				bool Min<Vector2DValueType>::_IsSmaller(const Vector2DValueType &a, const Vector2DValueType &b) const
				{
					return (a.SquareLength() < b.SquareLength());
				}

				template<>
				bool Min<Vector3DValueType>::_IsSmaller(const Vector3DValueType &a, const Vector3DValueType &b) const
				{
					return (a.SquareLength() < b.SquareLength());
				}

				template<>
				BooleanValueType Min<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				ColorARGBValueType Min<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				bool Max<BooleanValueType>::_IsBigger(const BooleanValueType &a, const BooleanValueType &b) const
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				bool Max<ColorARGBValueType>::_IsBigger(const ColorARGBValueType &a, const ColorARGBValueType &b) const
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				bool Max<Vector2DValueType>::_IsBigger(const Vector2DValueType &a, const Vector2DValueType &b) const
				{
					return (a.SquareLength() > b.SquareLength());
				}

				template<>
				bool Max<Vector3DValueType>::_IsBigger(const Vector3DValueType &a, const Vector3DValueType &b) const
				{
					return (a.SquareLength() > b.SquareLength());
				}

				template<>
				BooleanValueType Max<BooleanValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				template<>
				ColorARGBValueType Max<ColorARGBValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					throw new UIDescriptionExpressionTypeErrorException();
				}

				//------------------------------------------------------------------------------ Abs
				template<>
				IntegerValueType Abs<IntegerValueType, IntegerValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					IntegerValueType calculatedValue = m_operand->Evaluate(processingContext);
					if (calculatedValue < 0)
					{
						return -calculatedValue;
					}
					return calculatedValue;
				}

				template<>
				FloatingPointValueType Abs<FloatingPointValueType, FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return static_cast<FloatingPointValueType>(std::abs(m_operand->Evaluate(processingContext)));
				}

				template<>
				FloatingPointValueType Abs<Vector2DValueType, FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return static_cast<FloatingPointValueType>(m_operand->Evaluate(processingContext).Length());
				}

				template<>
				FloatingPointValueType Abs<Vector3DValueType, FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return static_cast<FloatingPointValueType>(m_operand->Evaluate(processingContext).Length());
				}

				//------------------------------------------------------------------------------ Power
				template<>
				FloatingPointValueType Power<FloatingPointValueType>::_CalculatePower(const FloatingPointValueType &base, const FloatingPointValueType &exponent) const
				{
					return static_cast<FloatingPointValueType>(pow(base, exponent));
				}

				template<>
				FloatingPointValueType Power<IntegerValueType>::_CalculatePower(const IntegerValueType &base, const IntegerValueType &exponent) const
				{
					return static_cast<FloatingPointValueType>(pow(base, exponent));
				}

				//------------------------------------------------------------------------------ Integerize Operators
				IntegerValueType Round::_Integerize(FloatingPointValueType sourceValue) const
				{
					return static_cast<IntegerValueType>(round(sourceValue));
				}

				IntegerValueType Floor::_Integerize(FloatingPointValueType sourceValue) const
				{
					return static_cast<IntegerValueType>(floor(sourceValue));
				}

				IntegerValueType Ceil::_Integerize(FloatingPointValueType sourceValue) const
				{
					return static_cast<IntegerValueType>(ceil(sourceValue));
				}

				//------------------------------------------------------------------------------ Texture
				template<>
				IntegerValueType TextureGetWidth<IntegerValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					ObjectEnvironmentPair foundTexture(processingContext.Find(m_targetTexturePath));
					if (!std::get<0>(foundTexture))
					{
						// Texture Not Found
						throw new UIDescriptionExpressionVariableNotExistException();
					}

					std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
					if (!foundTextureSegmentDescription)
					{
						// Invalid Texture
						throw new UIDescriptionExpressionTypeErrorException();
					}

					UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
					return static_cast<IntegerValueType>(foundTextureSegment.section.GetPixelWidth());
				}

				template<>
				FloatingPointValueType TextureGetWidth<FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					ObjectEnvironmentPair foundTexture(processingContext.Find(m_targetTexturePath));
					if (!std::get<0>(foundTexture))
					{
						// Texture Not Found
						throw new UIDescriptionExpressionVariableNotExistException();
					}

					std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
					if (!foundTextureSegmentDescription)
					{
						// Invalid Texture
						throw new UIDescriptionExpressionTypeErrorException();
					}

					UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
					return static_cast<FloatingPointValueType>(foundTextureSegment.section.GetPixelWidth());
				}

				template<>
				IntegerValueType TextureGetHeight<IntegerValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					ObjectEnvironmentPair foundTexture(processingContext.Find(m_targetTexturePath));
					if (!std::get<0>(foundTexture))
					{
						// Texture Not Found
						throw new UIDescriptionExpressionVariableNotExistException();
					}

					std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
					if (!foundTextureSegmentDescription)
					{
						// Invalid Texture
						throw new UIDescriptionExpressionTypeErrorException();
					}

					UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
					return static_cast<IntegerValueType>(foundTextureSegment.section.GetPixelHeight());
				}

				template<>
				FloatingPointValueType TextureGetHeight<FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					ObjectEnvironmentPair foundTexture(processingContext.Find(m_targetTexturePath));
					if (!std::get<0>(foundTexture))
					{
						// Texture Not Found
						throw new UIDescriptionExpressionVariableNotExistException();
					}

					std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
					if (!foundTextureSegmentDescription)
					{
						// Invalid Texture
						throw new UIDescriptionExpressionTypeErrorException();
					}

					UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
					return static_cast<FloatingPointValueType>(foundTextureSegment.section.GetPixelHeight());
				}

				//------------------------------------------------------------------------------ ComponentOf
				template<>
				IntegerValueType ComponentOf<ColorARGBValueType, IntegerValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					IntegerValueType indexing = m_index->Evaluate(processingContext);
					ColorARGBValueType colorValue = m_operand->Evaluate(processingContext);
					switch (indexing)
					{
						case 0:	return colorValue.alpha; // A
						case 1:	return colorValue.red; // R
						case 2:	return colorValue.green; // G
						case 3:	return colorValue.blue;// B
					}
					throw new UIDescriptionExpressionOutOfIndexException();
				}

				template<>
				FloatingPointValueType ComponentOf<Vector2DValueType, FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					IntegerValueType indexing = m_index->Evaluate(processingContext);
					Vector2DValueType vectorValue = m_operand->Evaluate(processingContext);
					switch (indexing)
					{
						case 0:	return vectorValue.x;
						case 1:	return vectorValue.y;
					}
					throw new UIDescriptionExpressionOutOfIndexException();
				}

				template<>
				FloatingPointValueType ComponentOf<Vector3DValueType, FloatingPointValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					IntegerValueType indexing = m_index->Evaluate(processingContext);
					Vector3DValueType vectorValue = m_operand->Evaluate(processingContext);
					switch (indexing)
					{
						case 0:	return vectorValue.x;
						case 1:	return vectorValue.y;
						case 2:	return vectorValue.z;
					}
					throw new UIDescriptionExpressionOutOfIndexException();
				}

				//------------------------------------------------------------------------------ Vector Operations
				FloatingPointValueType VectorDot::Evaluate(DescriptionProcessingContext &processingContext)
				{
					Vector3DValueType vector1 = m_vector1->Evaluate(processingContext);
					Vector3DValueType vector2 = m_vector2->Evaluate(processingContext);
					return static_cast<FloatingPointValueType>(vector1.GetDotWith(vector2));
				}

				Vector3DValueType VectorCross::Evaluate(DescriptionProcessingContext &processingContext)
				{
					Vector3DValueType vector1 = m_vector1->Evaluate(processingContext);
					Vector3DValueType vector2 = m_vector2->Evaluate(processingContext);
					return vector1.GetCrossWith(vector2);
				}

				template<>
				Vector2DValueType VectorScalarMultiply<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					Vector2DValueType vectorValue = m_vector->Evaluate(processingContext);
					FloatingPointValueType multiplier = m_multiplier->Evaluate(processingContext);
					return multiplier * vectorValue;
				}

				template<>
				Vector3DValueType VectorScalarMultiply<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					Vector3DValueType vectorValue = m_vector->Evaluate(processingContext);
					FloatingPointValueType multiplier = m_multiplier->Evaluate(processingContext);
					return multiplier * vectorValue;
				}

				template<>
				Vector2DValueType VectorNormal<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return m_vector->Evaluate(processingContext).GetNormalized();
				}

				template<>
				Vector3DValueType VectorNormal<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return m_vector->Evaluate(processingContext).GetNormalized();
				}

				template<>
				FloatingPointValueType VectorLength<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return m_vector->Evaluate(processingContext).Length();
				}

				template<>
				FloatingPointValueType VectorLength<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return m_vector->Evaluate(processingContext).Length();
				}

				template<>
				FloatingPointValueType VectorSquareLength<Vector2DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return m_vector->Evaluate(processingContext).SquareLength();
				}

				template<>
				FloatingPointValueType VectorSquareLength<Vector3DValueType>::Evaluate(DescriptionProcessingContext &processingContext)
				{
					return m_vector->Evaluate(processingContext).SquareLength();
				}
			}
		}
	}
}
