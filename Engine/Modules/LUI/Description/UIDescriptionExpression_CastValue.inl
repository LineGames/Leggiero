////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionVariable_CastValue.inl (Leggiero/Modules - LegacyUI)
//
// Inter-Type Casting Implementations
// Included in UIDescriptionExpression.cpp
////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////// Identity

template <>
BooleanValueType CastValue<BooleanValueType, BooleanValueType>(BooleanValueType valueToCast)
{
	return valueToCast;
}

template <>
IntegerValueType CastValue<IntegerValueType, IntegerValueType>(IntegerValueType valueToCast)
{
	return valueToCast;
}

template <>
FloatingPointValueType CastValue<FloatingPointValueType, FloatingPointValueType>(FloatingPointValueType valueToCast)
{
	return valueToCast;
}

template <>
ColorARGBValueType CastValue<ColorARGBValueType, ColorARGBValueType>(ColorARGBValueType valueToCast)
{
	return valueToCast;
}

template <>
Vector2DValueType CastValue<Vector2DValueType, Vector2DValueType>(Vector2DValueType valueToCast)
{
	return valueToCast;
}

template <>
Vector3DValueType CastValue<Vector3DValueType, Vector3DValueType>(Vector3DValueType valueToCast)
{
	return valueToCast;
}

template <>
StringValueType CastValue<StringValueType, StringValueType>(StringValueType valueToCast)
{
	return valueToCast;
}


//////////////////////////////////////////////////////////////////////////////// Other Castings

//------------------------------------------------------------------------------ to BooleanValueType
template <>
BooleanValueType CastValue<IntegerValueType, BooleanValueType>(IntegerValueType valueToCast)
{
	return !(valueToCast == 0);
}

template <>
BooleanValueType CastValue<StringValueType, BooleanValueType>(StringValueType valueToCast)
{
	std::tuple<BooleanValueType, bool> resultWithValidity = _Internal::_ParseBooleanValueFromStringWithValidity(valueToCast.c_str());
	if (!std::get<1>(resultWithValidity))
	{
		throw new UIDescriptionExpressionTypeErrorException();
	}

	return std::get<0>(resultWithValidity);
}


//------------------------------------------------------------------------------ to IntegerValueType
template <>
IntegerValueType CastValue<FloatingPointValueType, IntegerValueType>(FloatingPointValueType valueToCast)
{
	return static_cast<IntegerValueType>(valueToCast);
}

template <>
IntegerValueType CastValue<ColorARGBValueType, IntegerValueType>(ColorARGBValueType valueToCast)
{
	return static_cast<IntegerValueType>(valueToCast.ToColorCodeARGB());
}

template <>
IntegerValueType CastValue<StringValueType, IntegerValueType>(StringValueType valueToCast)
{
	if (valueToCast.length() < 1)
	{
		throw new UIDescriptionExpressionTypeErrorException();
	}

	return static_cast<IntegerValueType>(strtol(valueToCast.c_str(), nullptr, 0));
}


//------------------------------------------------------------------------------ to FloatingPointValueType
template <>
FloatingPointValueType CastValue<IntegerValueType, FloatingPointValueType>(IntegerValueType valueToCast)
{
	return static_cast<FloatingPointValueType>(valueToCast);
}

template <>
FloatingPointValueType CastValue<StringValueType, FloatingPointValueType>(StringValueType valueToCast)
{
	if (valueToCast.length() < 1)
	{
		throw new UIDescriptionExpressionTypeErrorException();
	}

	return static_cast<FloatingPointValueType>(strtof(valueToCast.c_str(), nullptr));
}


//------------------------------------------------------------------------------ to ColorARGBValueType
template <>
ColorARGBValueType CastValue<IntegerValueType, ColorARGBValueType>(IntegerValueType valueToCast)
{
	return ColorARGBValueType::FromColorCodeARGB(static_cast<ColorARGBValueType::ColorCodeType>(valueToCast));
}

template <>
ColorARGBValueType CastValue<StringValueType, ColorARGBValueType>(StringValueType valueToCast)
{
	return ParseColorValueFromString(valueToCast.c_str());
}


//------------------------------------------------------------------------------ to Vector2DValueType
template <>
Vector2DValueType CastValue<Vector3DValueType, Vector2DValueType>(Vector3DValueType valueToCast)
{
	return Vector2DValueType(valueToCast.x, valueToCast.y);
}

template <>
Vector2DValueType CastValue<StringValueType, Vector2DValueType>(StringValueType valueToCast)
{
	return ParseVector2DValueFromString(valueToCast.c_str());
}


//------------------------------------------------------------------------------ to Vector3DValueType
template <>
Vector3DValueType CastValue<Vector2DValueType, Vector3DValueType>(Vector2DValueType valueToCast)
{
	return Vector3DValueType::FromVector2D(valueToCast);
}

template <>
Vector3DValueType CastValue<StringValueType, Vector3DValueType>(StringValueType valueToCast)
{
	return ParseVector3DValueFromString(valueToCast.c_str());
}


//------------------------------------------------------------------------------ to StringValueType
template <>
StringValueType CastValue<BooleanValueType, StringValueType>(BooleanValueType valueToCast)
{
	return valueToCast ? "True" : "False";
}

template <>
StringValueType CastValue<IntegerValueType, StringValueType>(IntegerValueType valueToCast)
{
	return fmt::format("{0}", valueToCast);
}

template <>
StringValueType CastValue<FloatingPointValueType, StringValueType>(FloatingPointValueType valueToCast)
{
	return fmt::format("{0}", valueToCast);
}

template <>
StringValueType CastValue<ColorARGBValueType, StringValueType>(ColorARGBValueType valueToCast)
{
	return fmt::format("#{0:02x}{1:02x}{2:02x}{3:02x}", valueToCast.alpha, valueToCast.red, valueToCast.green, valueToCast.blue);
}

template <>
StringValueType CastValue<Vector2DValueType, StringValueType>(Vector2DValueType valueToCast)
{
	return fmt::format("vec2({0}, {1})", valueToCast.x, valueToCast.y);
}

template <>
StringValueType CastValue<Vector3DValueType, StringValueType>(Vector3DValueType valueToCast)
{
	return fmt::format("vec3({0}, {1}, {2})", valueToCast.x, valueToCast.y, valueToCast.z);
}
