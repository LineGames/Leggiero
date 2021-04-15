////////////////////////////////////////////////////////////////////////////////
// Math/SimpleGeometry.h (Leggiero - Utility)
//
// Simple Mathmatical Geometry Utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__MATH__SIMPLE_GEOMETRY_H
#define __UTILITY__MATH__SIMPLE_GEOMETRY_H


// Standard Library
#include <cmath>

// Leggiero.Utility
#include "SimpleMath.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
			namespace FloatingPointLongConstants
			{
				constexpr long double kPi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148L;
				constexpr long double kInversePi = 1.0L / kPi;

				constexpr long double kPiOver2 = kPi / 2.0L;
				constexpr long double kPiOver3 = kPi / 3.0L;
				constexpr long double kPiOver4 = kPi / 4.0L;
				constexpr long double kPiOver6 = kPi / 6.0L;

				constexpr long double kPiMul2 = kPi * 2.0L;
				constexpr long double kPiMul3 = kPi * 3.0L;
				constexpr long double kPiMul4 = kPi * 4.0L;

				constexpr long double kRad2Deg = 180.0L / kPi;
				constexpr long double kDeg2Rad = kPi / 180.0L;
			}


			template <typename T>
			class FloatingPointSimpleGeometry
			{
			public:
				using ValueType = T;

			public:	// Constants
				static constexpr T kPi = static_cast<T>(FloatingPointLongConstants::kPi);
				static constexpr T kInversePi = static_cast<T>(FloatingPointLongConstants::kInversePi);

				static constexpr T kPiOver2 = static_cast<T>(FloatingPointLongConstants::kPiOver2);
				static constexpr T kPiOver3 = static_cast<T>(FloatingPointLongConstants::kPiOver3);
				static constexpr T kPiOver4 = static_cast<T>(FloatingPointLongConstants::kPiOver4);
				static constexpr T kPiOver6 = static_cast<T>(FloatingPointLongConstants::kPiOver6);

				static constexpr T kPiMul2 = static_cast<T>(FloatingPointLongConstants::kPiMul2);
				static constexpr T kPiMul3 = static_cast<T>(FloatingPointLongConstants::kPiMul3);
				static constexpr T kPiMul4 = static_cast<T>(FloatingPointLongConstants::kPiMul4);

				static constexpr T kRad2Deg = static_cast<T>(FloatingPointLongConstants::kRad2Deg);
				static constexpr T kDeg2Rad = static_cast<T>(FloatingPointLongConstants::kDeg2Rad);

			public:
				static constexpr T Rad2Deg(T radian)
				{
					return radian * kRad2Deg;
				}

				static constexpr T Deg2Rad(T degree)
				{
					return degree * kDeg2Rad;
				}


				static T WrapAnglePi(T angle)
				{
					return Math::WrapMinMax<T>(angle, -kPi, kPi);
				}

				static T WrapAngle2Pi(T angle)
				{
					return Math::WrapMinMax<T>(angle, 0, kPiMul2);
				}

				static T OppositeAngle(T angle)
				{
					return WrapAnglePi(angle + kPi);
				}

				static T DeltaAngle(T angleTo, T angleFrom)
				{
					if (angleFrom > angleTo)
					{
						T delta = angleFrom - angleTo;
						if (delta > kPi)
						{
							return angleTo + kPiMul2 - angleFrom;
						}
						return -delta;
					}
					else
					{
						T delta = angleTo - angleFrom;
						if (delta > kPi)
						{
							return -(angleFrom + kPiMul2 - angleTo);
						}
						return delta;
					}
				}

				inline T AzimuthPi(T x, T y)
				{
					constexpr T kAxisEpsilon = static_cast<T>(0.00000000000001L);
					if (x > -kAxisEpsilon && x < kAxisEpsilon)
					{
						if (y >= static_cast<T>(0.0))
						{
							return kPiOver2;
						}
						return -kPiOver2;
					}
					return static_cast<T>(std::atan2l(y, x));
				}
			};
		}

		namespace Mathd
		{
			constexpr double kPi = Math::FloatingPointSimpleGeometry<double>::kPi;
			constexpr double kInversePi = static_cast<double>(Math::FloatingPointLongConstants::kInversePi);

			constexpr double kPiOver2 = static_cast<double>(Math::FloatingPointLongConstants::kPiOver2);
			constexpr double kPiOver3 = static_cast<double>(Math::FloatingPointLongConstants::kPiOver3);
			constexpr double kPiOver4 = static_cast<double>(Math::FloatingPointLongConstants::kPiOver4);
			constexpr double kPiOver6 = static_cast<double>(Math::FloatingPointLongConstants::kPiOver6);

			constexpr double kPiMul2 = static_cast<double>(Math::FloatingPointLongConstants::kPiMul2);
			constexpr double kPiMul3 = static_cast<double>(Math::FloatingPointLongConstants::kPiMul3);
			constexpr double kPiMul4 = static_cast<double>(Math::FloatingPointLongConstants::kPiMul4);

			constexpr double kRad2Deg = static_cast<double>(Math::FloatingPointLongConstants::kRad2Deg);
			constexpr double kDeg2Rad = static_cast<double>(Math::FloatingPointLongConstants::kDeg2Rad);


			constexpr double Rad2Deg(double radian)
			{
				return radian * kRad2Deg;
			}

			constexpr double Deg2Rad(double degree)
			{
				return degree * kDeg2Rad;
			}


			inline double WrapAnglePi(double angle)
			{
				return Math::WrapMinMax<double>(angle, -kPi, kPi);
			}

			inline double WrapAngle2Pi(double angle)
			{
				return Math::WrapMinMax<double>(angle, 0, kPiMul2);
			}

			inline double OppositeAngle(double angle)
			{
				return WrapAnglePi(angle + kPi);
			}

			inline double DeltaAngle(double angleTo, double angleFrom)
			{
				if (angleFrom > angleTo)
				{
					double delta = angleFrom - angleTo;
					if (delta > kPi)
					{
						return angleTo + kPiMul2 - angleFrom;
					}
					return -delta;
				}
				else
				{
					double delta = angleTo - angleFrom;
					if (delta > kPi)
					{
						return -(angleFrom + kPiMul2 - angleTo);
					}
					return delta;
				}
			}

			inline double AzimuthPi(double x, double y)
			{
				constexpr double kAxisEpsilon = 0.000000000001;
				if (x > -kAxisEpsilon && x < kAxisEpsilon)
				{
					if (y >= 0.0)
					{
						return kPiOver2;
					}
					return -kPiOver2;
				}
				return std::atan2(y, x);
			}
		}

		namespace Mathf
		{
			constexpr float kPi = static_cast<float>(Math::FloatingPointLongConstants::kPi);
			constexpr float kInversePi = static_cast<float>(Math::FloatingPointLongConstants::kInversePi);

			constexpr float kPiOver2 = static_cast<float>(Math::FloatingPointLongConstants::kPiOver2);
			constexpr float kPiOver3 = static_cast<float>(Math::FloatingPointLongConstants::kPiOver3);
			constexpr float kPiOver4 = static_cast<float>(Math::FloatingPointLongConstants::kPiOver4);
			constexpr float kPiOver6 = static_cast<float>(Math::FloatingPointLongConstants::kPiOver6);

			constexpr float kPiMul2 = static_cast<float>(Math::FloatingPointLongConstants::kPiMul2);
			constexpr float kPiMul3 = static_cast<float>(Math::FloatingPointLongConstants::kPiMul3);
			constexpr float kPiMul4 = static_cast<float>(Math::FloatingPointLongConstants::kPiMul4);

			constexpr float kRad2Deg = static_cast<float>(Math::FloatingPointLongConstants::kRad2Deg);
			constexpr float kDeg2Rad = static_cast<float>(Math::FloatingPointLongConstants::kDeg2Rad);


			constexpr float Rad2Deg(float radian)
			{
				return radian * kRad2Deg;
			}

			constexpr float Deg2Rad(float degree)
			{
				return degree * kDeg2Rad;
			}


			inline float WrapAnglePi(float angle)
			{
				return Math::WrapMinMax<float>(angle, -kPi, kPi);
			}

			inline float WrapAngle2Pi(float angle)
			{
				return Math::WrapMinMax<float>(angle, 0, kPiMul2);
			}

			inline float OppositeAngle(float angle)
			{
				return WrapAnglePi(angle + kPi);
			}

			inline float DeltaAngle(float angleTo, float angleFrom)
			{
				if (angleFrom > angleTo)
				{
					float delta = angleFrom - angleTo;
					if (delta > kPi)
					{
						return angleTo + kPiMul2 - angleFrom;
					}
					return -delta;
				}
				else
				{
					float delta = angleTo - angleFrom;
					if (delta > kPi)
					{
						return -(angleFrom + kPiMul2 - angleTo);
					}
					return delta;
				}
			}

			inline float AzimuthPi(float x, float y)
			{
				constexpr float kAxisEpsilon = 0.0000001f;
				if (x > -kAxisEpsilon && x < kAxisEpsilon)
				{
					if (y >= 0.0f)
					{
						return kPiOver2;
					}
					return -kPiOver2;
				}
				return std::atan2f(y, x);
			}
		}

		namespace Math
		{
			
		}
	}
}

#endif
