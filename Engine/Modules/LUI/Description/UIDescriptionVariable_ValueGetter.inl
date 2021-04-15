////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionVariable_ValueGetter.inl (Leggiero/Modules - LegacyUI)
//
// UI Description Varaible Value Getter Implementations with Type-Converting
// Included in UIDescriptionExpression.cpp
////////////////////////////////////////////////////////////////////////////////


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//------------------------------------------------------------------------------
			// Description Varaible
			template<> BooleanValueType BoolVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}

			template<> IntegerValueType BoolVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<BooleanValueType, IntegerValueType>(GetValue(processingContext));
			}

			template<> FloatingPointValueType BoolVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<BooleanValueType, FloatingPointValueType>(GetValue(processingContext));
			}

			template<> ColorARGBValueType BoolVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<BooleanValueType, ColorARGBValueType>(GetValue(processingContext));
			}

			template<> Vector2DValueType BoolVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<BooleanValueType, Vector2DValueType>(GetValue(processingContext));
			}

			template<> Vector3DValueType BoolVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<BooleanValueType, Vector3DValueType>(GetValue(processingContext));
			}

			template<> StringValueType BoolVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<BooleanValueType, StringValueType>(GetValue(processingContext));
			}


			template<> BooleanValueType IntVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<IntegerValueType, BooleanValueType>(GetValue(processingContext));
			}

			template<> IntegerValueType IntVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}

			template<> FloatingPointValueType IntVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<IntegerValueType, FloatingPointValueType>(GetValue(processingContext));
			}

			template<> ColorARGBValueType IntVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<IntegerValueType, ColorARGBValueType>(GetValue(processingContext));
			}

			template<> Vector2DValueType IntVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<IntegerValueType, Vector2DValueType>(GetValue(processingContext));
			}

			template<> Vector3DValueType IntVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<IntegerValueType, Vector3DValueType>(GetValue(processingContext));
			}

			template<> StringValueType IntVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<IntegerValueType, StringValueType>(GetValue(processingContext));
			}


			template<> BooleanValueType FloatVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<FloatingPointValueType, BooleanValueType>(GetValue(processingContext));
			}

			template<> IntegerValueType FloatVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<FloatingPointValueType, IntegerValueType>(GetValue(processingContext));
			}

			template<> FloatingPointValueType FloatVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}

			template<> ColorARGBValueType FloatVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<FloatingPointValueType, ColorARGBValueType>(GetValue(processingContext));
			}

			template<> Vector2DValueType FloatVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<FloatingPointValueType, Vector2DValueType>(GetValue(processingContext));
			}

			template<> Vector3DValueType FloatVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<FloatingPointValueType, Vector3DValueType>(GetValue(processingContext));
			}

			template<> StringValueType FloatVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<FloatingPointValueType, StringValueType>(GetValue(processingContext));
			}


			template<> BooleanValueType ColorVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<ColorVariable, BooleanValueType>(GetValue(processingContext));
			}

			template<> IntegerValueType ColorVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<ColorVariable, IntegerValueType>(GetValue(processingContext));
			}

			template<> FloatingPointValueType ColorVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<ColorVariable, FloatingPointValueType>(GetValue(processingContext));
			}

			template<> ColorARGBValueType ColorVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}

			template<> Vector2DValueType ColorVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<ColorARGBValueType, Vector2DValueType>(GetValue(processingContext));
			}

			template<> Vector3DValueType ColorVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<ColorARGBValueType, Vector3DValueType>(GetValue(processingContext));
			}

			template<> StringValueType ColorVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<ColorVariable, StringValueType>(GetValue(processingContext));
			}


			template<> BooleanValueType Vector2DVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector2DValueType, BooleanValueType>(GetValue(processingContext));
			}

			template<> IntegerValueType Vector2DVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector2DValueType, IntegerValueType>(GetValue(processingContext));
			}

			template<> FloatingPointValueType Vector2DVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector2DValueType, FloatingPointValueType>(GetValue(processingContext));
			}

			template<> ColorARGBValueType Vector2DVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector2DValueType, ColorARGBValueType>(GetValue(processingContext));
			}

			template<> Vector2DValueType Vector2DVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}

			template<> Vector3DValueType Vector2DVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector2DValueType, Vector3DValueType>(GetValue(processingContext));
			}

			template<> StringValueType Vector2DVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector2DValueType, StringValueType>(GetValue(processingContext));
			}


			template<> BooleanValueType Vector3DVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector3DValueType, BooleanValueType>(GetValue(processingContext));
			}

			template<> IntegerValueType Vector3DVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector3DValueType, IntegerValueType>(GetValue(processingContext));
			}

			template<> FloatingPointValueType Vector3DVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector3DValueType, FloatingPointValueType>(GetValue(processingContext));
			}

			template<> ColorARGBValueType Vector3DVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector3DValueType, ColorARGBValueType>(GetValue(processingContext));
			}

			template<> Vector2DValueType Vector3DVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector3DValueType, Vector2DValueType>(GetValue(processingContext));
			}

			template<> Vector3DValueType Vector3DVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}

			template<> StringValueType Vector3DVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<Vector3DValueType, StringValueType>(GetValue(processingContext));
			}


			template<> BooleanValueType StringVariable::GetValueBoolean(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<StringValueType, BooleanValueType>(GetValue(processingContext));
			}

			template<> IntegerValueType StringVariable::GetValueInteger(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<StringValueType, IntegerValueType>(GetValue(processingContext));
			}

			template<> FloatingPointValueType StringVariable::GetValueFloatingPoint(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<StringValueType, FloatingPointValueType>(GetValue(processingContext));
			}

			template<> ColorARGBValueType StringVariable::GetValueColor(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<StringValueType, ColorARGBValueType>(GetValue(processingContext));
			}

			template<> Vector2DValueType StringVariable::GetValueVector2D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<StringValueType, Vector2DValueType>(GetValue(processingContext));
			}

			template<> Vector3DValueType StringVariable::GetValueVector3D(DescriptionProcessingContext &processingContext)
			{
				return Expression::_Internal::CastValue<StringValueType, Vector3DValueType>(GetValue(processingContext));
			}

			template<> StringValueType StringVariable::GetValueString(DescriptionProcessingContext &processingContext)
			{
				return GetValue(processingContext);
			}
		}
	}
}
