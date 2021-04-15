////////////////////////////////////////////////////////////////////////////////
// Common/GLColor.cpp (Leggiero/Modules - Graphics)
//
// Color Utility Implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLColor.h"

// Standard Library
#include <cmath>
#include <limits>


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// HSLColor

		namespace HSLColorUtil
		{
			//------------------------------------------------------------------------------
			float _Hue2RGB(float p, float q, float t)
			{
				constexpr float kOneOverSix = 1.0f / 6.0f;
				constexpr float kTwoOverThree = 2.0f / 3.0f;

				if (t < 0.0f) t += 1.0f;
				if (t > 1.0f) t -= 1.0f;
				if (t < kOneOverSix) return p + (q - p) * 6.0f * t;
				if (t < 0.5f) return q;
				if (t < kTwoOverThree) return p + (q - p) * (kTwoOverThree - t) * 6.0f;
				return p;
			}
		}


		//------------------------------------------------------------------------------
		HSLColor HSLColor::FromRGB(float r, float g, float b)
		{
			float max = (float)fmax(r, fmax(g, b));
			float min = (float)fmin(r, fmin(g, b));
			float l = (max + min) * 0.5f;

			if (max == min)
			{
				return HSLColor(0.0f, 0.0f, l);
			}

			float d = max - min;
			float s = (l > 0.5f) ? (d / (2.0f - max - min)) : (d / (max + min));

			float h;
			if (max == r)
			{
				h = (g - b) / d + (g < b ? 6.0f : 0.0f);
			}
			else if (max == g)
			{
				h = (b - r) / d + 2.0f;
			}
			else
			{
				h = (r - g) / d + 4.0f;
			}

			constexpr float kOneOverSix = 1.0f / 6.0f;
			h *= kOneOverSix;

			return HSLColor(h, s, l);
		}

		//------------------------------------------------------------------------------
		HSLColor HSLColor::FromRGB(const GLByteRGB &rgb)
		{
			return FromRGB((float)(rgb.red / 255.0f), (float)(rgb.green / 255.0f), (float)(rgb.blue / 255.0f));
		}

		//------------------------------------------------------------------------------
		GLByteRGB HSLColor::ToRGB() const
		{
			// Code From http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
			if (saturation <= 0.0f)
			{
				GLubyte byteLightness = (GLubyte)round(lightness * 255.0f);
				return GLByteRGB(byteLightness, byteLightness, byteLightness);
			}

			float q = (lightness < 0.5f) ? (lightness * (1.0f + saturation)) : (lightness + saturation - lightness * saturation);
			float p = 2.0f * lightness - q;
			float r = HSLColorUtil::_Hue2RGB(p, q, hue + 0.333333f);
			float g = HSLColorUtil::_Hue2RGB(p, q, hue);
			float b = HSLColorUtil::_Hue2RGB(p, q, hue - 0.333333f);

			return GLByteRGB((GLubyte)round(r * 255.0f), (GLubyte)round(g * 255.0f), (GLubyte)round(b * 255.0f));
		}

		//------------------------------------------------------------------------------
		void HSLColor::HueShift(float amount)
		{
			hue = (float)(fmod(hue + amount, 1.0f));
		}

		//------------------------------------------------------------------------------
		void HSLColor::SaturationAdjust(float amount)
		{
			float resultValue = saturation + amount;
			if (resultValue < 0.0f)
			{
				resultValue = 0.0f;
			}
			else if (resultValue > 1.0f)
			{
				resultValue = 1.0f;
			}
			saturation = resultValue;
		}

		//------------------------------------------------------------------------------
		void HSLColor::SaturationMultiply(float amount)
		{
			float resultValue = saturation * amount;
			if (resultValue < 0.0f)
			{
				resultValue = 0.0f;
			}
			else if (resultValue > 1.0f)
			{
				resultValue = 1.0f;
			}
			saturation = resultValue;
		}

		//------------------------------------------------------------------------------
		void HSLColor::LightnessAdjust(float amount)
		{
			float resultValue = lightness + amount;
			if (resultValue < 0.0f)
			{
				resultValue = 0.0f;
			}
			else if (resultValue > 1.0f)
			{
				resultValue = 1.0f;
			}
			lightness = resultValue;
		}

		//------------------------------------------------------------------------------
		void HSLColor::SetHue(float value)
		{
			hue = (float)fmod(value, 1.0f);
		}

		//------------------------------------------------------------------------------
		void HSLColor::SetSaturation(float value)
		{
			if (value < 0.0f)
			{
				value = 0.0f;
			}
			else if (value > 1.0f)
			{
				value = 1.0f;
			}
			saturation = value;
		}

		//------------------------------------------------------------------------------
		void HSLColor::SetLightness(float value)
		{
			if (value < 0.0f)
			{
				value = 0.0f;
			}
			else if (value > 1.0f)
			{
				value = 1.0f;
			}
			lightness = value;
		}


		//////////////////////////////////////////////////////////////////////////////// GLByteRGB

		//------------------------------------------------------------------------------
		const GLByteRGB GLByteRGB::kBlack((GLubyte)0, (GLubyte)0, (GLubyte)0);
		const GLByteRGB GLByteRGB::kWhite((GLubyte)255, (GLubyte)255, (GLubyte)255);
		const GLByteRGB GLByteRGB::kRed((GLubyte)255, (GLubyte)0, (GLubyte)0);
		const GLByteRGB GLByteRGB::kGreen((GLubyte)0, (GLubyte)255, (GLubyte)0);
		const GLByteRGB GLByteRGB::kBlue((GLubyte)0, (GLubyte)0, (GLubyte)255);

		//------------------------------------------------------------------------------
		GLByteRGB GLByteRGB::Grey(GLubyte brightness)
		{
			return GLByteRGB(brightness, brightness, brightness);
		}

		//------------------------------------------------------------------------------
		GLByteRGB GLByteRGB::Grey(int brightness)
		{
			return Grey((GLubyte)((brightness < 0) ? 0 : ((brightness > 255) ? 255 : brightness)));
		}

		//------------------------------------------------------------------------------
		GLByteRGB GLByteRGB::Grey(float brightness)
		{
			return Grey((GLubyte)((brightness < 0.0f) ? 0.0f : ((brightness > 1.0f) ? 255.0f : (GLubyte)(brightness * 255.0f))));
		}

		//------------------------------------------------------------------------------
		// Create a color based on a Color value in range [0.0, 1.0]
		// Same as HSV Color with S=100%, V=100%, for [0 Degree, 360 Degree] mapped from [0.0, 1.0]
		GLByteRGB GLByteRGB::ChromaColor(float chroma)
		{
			if (!std::isfinite(chroma))
			{
				return kBlack;
			}
			if (chroma > 1.0f || chroma < 0.0f)
			{
				chroma = fmod(chroma, 1.0f);
				if (chroma < 0.0f)
				{
					chroma = 1.0f - chroma;
				}
			}
			constexpr float kOneOverSix = 1.0f / 6.0f;
			if (chroma <= kOneOverSix)
			{
				return GLByteRGB((GLubyte)255, (GLubyte)(255.0f * chroma * 6.0f), (GLubyte)0);
			}
			else if (chroma <= kOneOverSix * 2.0f)
			{
				return GLByteRGB((GLubyte)(255.0f * (1.0f - (chroma - kOneOverSix) * 6.0f)), (GLubyte)255, (GLubyte)0);
			}
			else if (chroma <= kOneOverSix * 3.0f)
			{
				return GLByteRGB((GLubyte)0, (GLubyte)255, (GLubyte)(255.0f * (chroma - kOneOverSix * 2.0f) * 6.0f));
			}
			else if (chroma <= kOneOverSix * 4.0f)
			{
				return GLByteRGB((GLubyte)0, (GLubyte)(255.0f * (1.0f - (chroma - kOneOverSix * 3.0f) * 6.0f)), (GLubyte)255);
			}
			else if (chroma <= kOneOverSix * 5.0f)
			{
				return GLByteRGB((GLubyte)(255.0f * (chroma - kOneOverSix * 4.0f) * 6.0f), (GLubyte)0, (GLubyte)255);
			}
			else
			{
				return GLByteRGB((GLubyte)255, (GLubyte)0, (GLubyte)(255.0f * (1.0f - (chroma - kOneOverSix * 5.0f) * 6.0f)));
			}
		}

		//------------------------------------------------------------------------------
		GLByteRGB GLByteRGB::FromHSL(float h, float s, float l)
		{
			return HSLColor(h, s, l).ToRGB();
		}

		//------------------------------------------------------------------------------
		GLByteRGB GLByteRGB::FromHSL(const HSLColor &hsl)
		{
			return hsl.ToRGB();
		}

		//------------------------------------------------------------------------------
		HSLColor GLByteRGB::ToHSL() const
		{
			return HSLColor::FromRGB((float)(red / 255.0f), (float)(green / 255.0f), (float)(blue / 255.0f));
		}


		//////////////////////////////////////////////////////////////////////////////// GLByteARGB

		//------------------------------------------------------------------------------
		const GLByteARGB GLByteARGB::kTransparent((GLubyte)0, (GLubyte)0, (GLubyte)0, (GLubyte)0);
		const GLByteARGB GLByteARGB::kBlack((GLubyte)255, (GLubyte)0, (GLubyte)0, (GLubyte)0);
		const GLByteARGB GLByteARGB::kWhite((GLubyte)255, (GLubyte)255, (GLubyte)255, (GLubyte)255);
		const GLByteARGB GLByteARGB::kRed((GLubyte)255, (GLubyte)255, (GLubyte)0, (GLubyte)0);
		const GLByteARGB GLByteARGB::kGreen((GLubyte)255, (GLubyte)0, (GLubyte)255, (GLubyte)0);
		const GLByteARGB GLByteARGB::kBlue((GLubyte)255, (GLubyte)0, (GLubyte)0, (GLubyte)255);

		//------------------------------------------------------------------------------
		GLByteARGB::ColorCodeType GLByteARGB::ToColorCodeARGB() const
		{
			return ((alpha << 24) | (red << 16) | (green << 8) | blue);
		}

		//------------------------------------------------------------------------------
		GLByteARGB::ColorCodeType GLByteARGB::ToColorCodeRGBA() const
		{
			return ((red << 24) | (green << 16) | (blue << 8) | alpha);
		}

		//------------------------------------------------------------------------------
		GLByteARGB GLByteARGB::FromColorCodeARGB(GLByteARGB::ColorCodeType colorCode)
		{
			return GLByteARGB(
				(GLubyte)((colorCode >> 24) & 0xffU),
				(GLubyte)((colorCode >> 16) & 0xffU),
				(GLubyte)((colorCode >> 8) & 0xffU),
				(GLubyte)(colorCode & 0xffU)
			);
		}

		//------------------------------------------------------------------------------
		GLByteARGB GLByteARGB::FromColorCodeRGBA(GLByteARGB::ColorCodeType colorCode)
		{
			return GLByteARGB(
				(GLubyte)(colorCode & 0xffU),
				(GLubyte)((colorCode >> 24) & 0xffU),
				(GLubyte)((colorCode >> 16) & 0xffU),
				(GLubyte)((colorCode >> 8) & 0xffU)
			);
		}

		//------------------------------------------------------------------------------
		GLByteARGB GLByteARGB::Grey(GLubyte brightness, GLubyte alpha)
		{
			return GLByteARGB(brightness, brightness, brightness, alpha);
		}

		//------------------------------------------------------------------------------
		GLByteARGB GLByteARGB::Grey(int brightness, int alpha)
		{
			return Grey((GLubyte)((brightness < 0) ? 0 : ((brightness > 255) ? 255 : brightness)), (GLubyte)((alpha < 0) ? 0 : ((alpha > 255) ? 255 : alpha)));
		}

		//------------------------------------------------------------------------------
		GLByteARGB GLByteARGB::Grey(float brightness, float alpha)
		{
			return Grey((GLubyte)((brightness < 0.0f) ? 0.0f : ((brightness > 1.0f) ? 255.0f : (GLubyte)(brightness * 255.0f))), (GLubyte)((alpha < 0.0f) ? 0.0f : ((alpha > 1.0f) ? 255.0f : (GLubyte)(alpha * 255.0f))));
		}

		//------------------------------------------------------------------------------
		// Create a color based on a Color value in range [0.0, 1.0]
		// Same as HSV Color with S=100%, V=100%, for [0 Degree, 360 Degree] mapped from [0.0, 1.0]
		GLByteARGB GLByteARGB::ChromaColor(float chroma, float alpha)
		{
			return GLByteARGB(GLByteRGB::ChromaColor(chroma), (GLubyte)((alpha < 0.0f) ? 0.0f : ((alpha > 1.0f) ? 255.0f : (GLubyte)(alpha * 255.0f))));
		}
	}
}
