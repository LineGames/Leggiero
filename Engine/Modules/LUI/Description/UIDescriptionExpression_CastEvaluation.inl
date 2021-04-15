////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionExpression_CastEvaluation.inl (Leggiero/Modules - LegacyUI)
//
// Cast<FromT, ToT>::Evaluate functions' template specialization macros
// Included in UIDescriptionExpression.cpp
////////////////////////////////////////////////////////////////////////////////


_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, BooleanValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, BooleanValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, BooleanValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, BooleanValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, BooleanValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, BooleanValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, BooleanValueType)

_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, IntegerValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, IntegerValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, IntegerValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, IntegerValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, IntegerValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, IntegerValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, IntegerValueType)

_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, FloatingPointValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, FloatingPointValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, FloatingPointValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, FloatingPointValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, FloatingPointValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, FloatingPointValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, FloatingPointValueType)

_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, ColorARGBValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, ColorARGBValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, ColorARGBValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, ColorARGBValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, ColorARGBValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, ColorARGBValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, ColorARGBValueType)

_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, Vector2DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, Vector2DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, Vector2DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, Vector2DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, Vector2DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, Vector2DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, Vector2DValueType)

_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, Vector3DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, Vector3DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, Vector3DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, Vector3DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, Vector3DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, Vector3DValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, Vector3DValueType)

_MAKE_CAST_EVAL_FROM_TEMPLATE(BooleanValueType, StringValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(IntegerValueType, StringValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(FloatingPointValueType, StringValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(ColorARGBValueType, StringValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector2DValueType, StringValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(Vector3DValueType, StringValueType)
_MAKE_CAST_EVAL_FROM_TEMPLATE(StringValueType, StringValueType)
