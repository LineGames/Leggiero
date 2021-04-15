////////////////////////////////////////////////////////////////////////////////
// Math/Easing.cpp (Leggiero - Utility)
//
// Easing Functions Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Easing.h"

// Standard Library
#include <cmath>

// Leggiero.Utility
#include "SimpleMath.h"
#include "SimpleGeometry.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Math
		{
			//////////////////////////////////////////////////////////////////////////////// Cubic Bezier Easing Implementation

			// This implementation is a C++ version of bezier-easing library by gre
			// https://github.com/gre/bezier-easing (distributed under MIT License)

			namespace _Internal
			{
                constexpr int kNEWTON_ITERATIONS = 4;
                constexpr double kNEWTON_MIN_SLOPE = 0.001;
                constexpr double kSUBDIVISION_PRECISION = 0.0000001;
                constexpr int kSUBDIVISION_MAX_ITERATIONS = 10;

                constexpr double kSampleStepSize = 1.0 / (CubicBezierEasing::kSplineTableSize - 1.0);

                constexpr double A(double aA1, double aA2)
                {
                    return 1.0 - 3.0 * aA2 + 3.0 * aA1;
                }

                constexpr double B(double aA1, double aA2)
                {
                    return 3.0 * aA2 - 6.0 * aA1;
                }

                constexpr double C(double aA1)
                {
                    return 3.0 * aA1;
                }

                constexpr double calcBezier(double aT, double aA1, double aA2)
                {
                    return ((A(aA1, aA2) * aT + B(aA1, aA2)) * aT + C(aA1)) * aT;
                }

                constexpr double getSlope(double aT, double aA1, double aA2)
                {
                    return 3.0 * A(aA1, aA2) * aT * aT + 2.0 * B(aA1, aA2) * aT + C(aA1);
                }

                double binarySubdivide(double aX, double aA, double aB, double mX1, double mX2)
                {
                    double currentX = 0.0;
                    double currentT = 0.0;
                    int i = 0;
                    do
                    {
                        currentT = aA + (aB - aA) / 2.0;
                        currentX = calcBezier(currentT, mX1, mX2) - aX;
                        if (currentX > 0.0)
                        {
                            aB = currentT;
                        }
                        else
                        {
                            aA = currentT;
                        }
                    } while (abs(currentX) > kSUBDIVISION_PRECISION && ++i < kSUBDIVISION_MAX_ITERATIONS);
                    return currentT;
                }

                double newtonRaphsonIterate(double aX, double aGuessT, double mX1, double mX2)
                {
                    for (int i = 0; i < kNEWTON_ITERATIONS; ++i)
                    {
                        double currentSlope = getSlope(aGuessT, mX1, mX2);
                        if (currentSlope == 0.0)
                        {
                            return aGuessT;
                        }
                        double currentX = calcBezier(aGuessT, mX1, mX2) - aX;
                        aGuessT -= currentX / currentSlope;
                    }
                    return aGuessT;
                }

                double getTForX(double aX, double mX1, double mX2, double sampleValues[])
                {
                    double intervalStart = 0.0;
                    int currentSample = 1;
                    int lastSample = CubicBezierEasing::kSplineTableSize - 1;

                    for (; currentSample != lastSample && sampleValues[currentSample] <= aX; ++currentSample)
                    {
                        intervalStart += kSampleStepSize;
                    }
                    --currentSample;

                    // Interpolate to provide an initial guess for t
                    double dist = (aX - sampleValues[currentSample]) / (sampleValues[currentSample + 1] - sampleValues[currentSample]);
                    double guessForT = intervalStart + dist * kSampleStepSize;

                    double initialSlope = getSlope(guessForT, mX1, mX2);
                    if (initialSlope >= kNEWTON_MIN_SLOPE)
                    {
                        return newtonRaphsonIterate(aX, guessForT, mX1, mX2);
                    }
                    else if (initialSlope == 0.0)
                    {
                        return guessForT;
                    }
                    return binarySubdivide(aX, intervalStart, intervalStart + kSampleStepSize, mX1, mX2);
                }
			}

            //------------------------------------------------------------------------------
            CubicBezierEasing::CubicBezierEasing(double p1x, double p1y, double p2x, double p2y)
                : m_p1x(ClipMinMax(p1x, 0.0, 1.0)), m_p1y(p1y), m_p2x(ClipMinMax(p2x, 0.0, 1.0)), m_p2y(p2y)
            {
                m_isLinearEasing = ((m_p1x == m_p1y) && (m_p2x == m_p2y));
                if (m_isLinearEasing)
                {
                    return;
                }

                for (int i = 0; i < kSplineTableSize; ++i)
                {
                    m_sampleValues[i] = _Internal::calcBezier(i * _Internal::kSampleStepSize, m_p1x, m_p2x);
                }
            }

            //------------------------------------------------------------------------------
            double CubicBezierEasing::Easy(double param)
            {
                if (m_isLinearEasing)
                {
                    return param;
                }
                if (param <= 0.0)
                {
                    return 0.0;
                }
                if (param >= 1.0)
                {
                    return 1.0;
                }
                return _Internal::calcBezier(_Internal::getTForX(param, m_p1x, m_p2x, m_sampleValues), m_p1y, m_p2y);
            }

			//------------------------------------------------------------------------------
			float EasyCubicBezier(float param, float p1x, float p1y, float p2x, float p2y)
			{
                if (param <= 0.0f)
                {
                    return 0.0f;
                }
                if (param >= 1.0f)
                {
                    return 1.0f;
                }
                if ((p1x == p1y) && (p2x == p2y))
                {
                    return param;
                }

                double sampleValues[CubicBezierEasing::kSplineTableSize];
                for (int i = 0; i < CubicBezierEasing::kSplineTableSize; ++i)
                {
                    sampleValues[i] = _Internal::calcBezier(i * _Internal::kSampleStepSize, p1x, p2x);
                }

                return static_cast<float>(_Internal::calcBezier(_Internal::getTForX(param, p1x, p2x, sampleValues), p1y, p2y));
			}


            //////////////////////////////////////////////////////////////////////////////// Bulge Easing Implementation

            namespace _Internal
            {
                namespace BulgeEasingConsts
                {
                    static const float kRoot2 = sqrtf(2.0f);
                    static const float kRoot3 = sqrtf(3.0f);
                    static const float kRoot6 = sqrtf(6.0f);

                    static const float kX = (3.0f - kRoot3) * 0.25f;
                    static const float kY = (kRoot3 - 1.0f) * 0.25f;
                    static const float k1Slow = kX;
                    static const float k2Slow = 1.0f - kY;
                    static const float k1Fast = kY;
                    static const float k2Fast = 1.0f - kX;

                    static const float kCircleR = kRoot2 * 0.5f;
                    static const float kCircleRInv = 1.0f / kCircleR;

                    static const float kSideCircleCenterAmount = 0.5f;

                    static const float kMiddleCircleCenterAmount = (kRoot3 - 1.0f) * 0.5f;
                    static const float kMiddleCircleCenterXSlow = 0.5f - kMiddleCircleCenterAmount;
                    static const float kMiddleCircleCenterYSlow = 0.5f + kMiddleCircleCenterAmount;
                    static const float kMiddleCircleCenterXFast = 0.5f + kMiddleCircleCenterAmount;
                    static const float kMiddleCircleCenterYFast = 0.5f - kMiddleCircleCenterAmount;
                }
            }

            //------------------------------------------------------------------------------
            // Smooth Fast-and-Go
            float EasySmoothBulgeFast(float param)
            {
                if (param <= 0.0f)
                {
                    return 0.0f;
                }
                if (param >= 1.0f)
                {
                    return 1.0f;
                }

                if (param < _Internal::BulgeEasingConsts::k1Fast)
                {
                    float rx = param + _Internal::BulgeEasingConsts::kSideCircleCenterAmount;
                    float theta = acosf(rx * _Internal::BulgeEasingConsts::kCircleRInv);
                    return _Internal::BulgeEasingConsts::kSideCircleCenterAmount - _Internal::BulgeEasingConsts::kCircleR * sinf(theta);
                }
                else if (param < _Internal::BulgeEasingConsts::k2Fast)
                {
                    float rx = _Internal::BulgeEasingConsts::kMiddleCircleCenterXFast - param;
                    float theta = acosf(rx * _Internal::BulgeEasingConsts::kCircleRInv);
                    return _Internal::BulgeEasingConsts::kMiddleCircleCenterYFast + _Internal::BulgeEasingConsts::kCircleR * sinf(theta);
                }
                else
                {
                    float rx = param - 1.0f + _Internal::BulgeEasingConsts::kSideCircleCenterAmount;
                    float theta = acosf(rx * _Internal::BulgeEasingConsts::kCircleRInv);
                    return 1.0f + _Internal::BulgeEasingConsts::kSideCircleCenterAmount - _Internal::BulgeEasingConsts::kCircleR * sinf(theta);
                }
            }

            //------------------------------------------------------------------------------
            // Smooth Slow-and-Go
            float EasySmoothBulgeSlow(float param)
            {
                if (param <= 0.0f)
                {
                    return 0.0f;
                }
                if (param >= 1.0f)
                {
                    return 1.0f;
                }

                if (param < _Internal::BulgeEasingConsts::k1Slow)
                {
                    float rx = _Internal::BulgeEasingConsts::kSideCircleCenterAmount - param;
                    float theta = acosf(rx * _Internal::BulgeEasingConsts::kCircleRInv);
                    return _Internal::BulgeEasingConsts::kCircleR * sinf(theta) - _Internal::BulgeEasingConsts::kSideCircleCenterAmount;
                }
                else if (param < _Internal::BulgeEasingConsts::k2Slow)
                {
                    float rx = param - _Internal::BulgeEasingConsts::kMiddleCircleCenterXSlow;
                    float theta = acosf(rx * _Internal::BulgeEasingConsts::kCircleRInv);
                    return _Internal::BulgeEasingConsts::kMiddleCircleCenterYSlow - _Internal::BulgeEasingConsts::kCircleR * sinf(theta);
                }
                else
                {
                    float rx = 1.0f + _Internal::BulgeEasingConsts::kSideCircleCenterAmount - param;
                    float theta = acosf(rx * _Internal::BulgeEasingConsts::kCircleRInv);
                    return _Internal::BulgeEasingConsts::kCircleR * sinf(theta) + 1.0f - _Internal::BulgeEasingConsts::kSideCircleCenterAmount;
                }
            }
		}
	}
}
