////////////////////////////////////////////////////////////////////////////////
// Math/SimpleMath.h (Leggiero - Utility)
//
// Simple Mathmatical Utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__MATH__SIMPLE_MATH_H
#define __UTILITY__MATH__SIMPLE_MATH_H


// Standard Library
#include <cmath>


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
            template <typename ValueT>
            constexpr ValueT ClipMin(ValueT x, ValueT min)
            {
                return ((x < min) ? min : x);
            }

            template <typename ValueT>
            constexpr ValueT ClipMax(ValueT x, ValueT max)
            {
                return ((x > max) ? max : x);
            }

            template <typename ValueT>
            constexpr ValueT ClipMinMax(ValueT x, ValueT min, ValueT max)
            {
                return ((x < min) ? min : ((x > max) ? max : x));
            }

            template <typename ValueT>
            inline ValueT WrapMax(ValueT x, ValueT max)
            {
                return static_cast<ValueT>(std::fmod(max + std::fmod(x, max), max));
            }

            template <typename ValueT>
            inline ValueT WrapMinMax(ValueT x, ValueT min, ValueT max)
            {
                return min + WrapMax(x - min, max - min);
            }


            template <typename ValueT>
            constexpr ValueT SmallestGreaterEqualPow2(ValueT x)
            {
                int resultValue = 1;
                while (resultValue < x)
                {
                    resultValue *= 2;
                }
                return resultValue;
            }


            template <typename ValueT, typename ParamT>
            constexpr ValueT LerpValue(ValueT v1, ValueT v2, ParamT param)
            {
                return static_cast<ValueT>(v1 * (static_cast<ParamT>(1.0) - param) + v2 * param);
            }
		}
	}
}

#endif
