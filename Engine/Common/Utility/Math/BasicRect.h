////////////////////////////////////////////////////////////////////////////////
// Math/BasicRect.h (Leggiero - Utility)
//
// Basic Rectangulars
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__MATH__BASIC_RECT_H
#define __UTILITY__MATH__BASIC_RECT_H


// Standard Library
#include <cmath>
#include <limits>


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
			//////////////////////////////////////////////////////////////////////////////// BasicRect<CoordinateT>

			// Axis-aligned 2D Simple Rectangular
			template <typename CoordinateT>
			class BasicRect
			{
			public:
				CoordinateT left;
				CoordinateT top;
				CoordinateT right;
				CoordinateT bottom;

				using CoordinateType = CoordinateT;

			public:
				CoordinateT GetWidth() const { return right - left; }
				CoordinateT GetHeight() const { return bottom - top; }

			public:
				BasicRect(CoordinateT left = static_cast<CoordinateT>(0.0), CoordinateT top = static_cast<CoordinateT>(0.0), CoordinateT right = static_cast<CoordinateT>(0.0), CoordinateT bottom = static_cast<CoordinateT>(0.0))
					: left(left), top(top), right(right), bottom(bottom)
				{ }

				static BasicRect<CoordinateT> FromPositionSize(CoordinateT left, CoordinateT top, CoordinateT width, CoordinateT height)
				{
					return BasicRect<CoordinateT>(left, top, left + width, top + height);
				}
			};
		}
	}
}

#endif
