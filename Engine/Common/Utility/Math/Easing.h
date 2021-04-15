////////////////////////////////////////////////////////////////////////////////
// Math/Easing.h (Leggiero - Utility)
//
// Additional Easing Function Defines
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__MATH__EASING_H
#define __UTILITY__MATH__EASING_H


// Standard Library
#include <cmath>
#include <limits>


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
			// Cubic Bezier Easing Class
			// This is an object because it uses precomputed table.
			class CubicBezierEasing
			{
			public:
				CubicBezierEasing(double p1x, double p1y, double p2x, double p2y);

			public:
				double Easy(double param);

			public:
				const double m_p1x;
				const double m_p1y;
				const double m_p2x;
				const double m_p2y;

				static constexpr size_t kSplineTableSize = 11;

			private:
				bool m_isLinearEasing;
				double m_sampleValues[kSplineTableSize];
			};

			// Easy as cubic-bezier.
			// This is slow! Consider to use CubicBezierEasing class in production.
			float EasyCubicBezier(float param, float p1x, float p1y, float p2x, float p2y);

			// Get linear parameter in an interval
			template <typename ValueT>
			inline ValueT ParameterInInterval(ValueT rawParam, ValueT intervalStart, ValueT intervalFinish)
			{
				if (intervalStart > intervalFinish)
				{
					// Invalid Interval
					return std::numeric_limits<ValueT>::quiet_NaN();
				}
				if (rawParam <= intervalStart)
				{
					return static_cast<ValueT>(0.0);
				}
				if (rawParam >= intervalFinish)
				{
					return static_cast<ValueT>(1.0);
				}
				return (rawParam - intervalStart) / (intervalFinish - intervalStart);
			}

			// Smooth Fast-and-Go
			float EasySmoothBulgeFast(float param);

			// Smooth Slow-and-Go
			float EasySmoothBulgeSlow(float param);
		}
	}
}

#endif
