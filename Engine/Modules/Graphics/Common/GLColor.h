////////////////////////////////////////////////////////////////////////////////
// Common/GLColor.h (Leggiero/Modules - Graphics)
//
// Provide Utilities for OpenGL Color
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__COMMON__GL_COLOR_H
#define __LM_GRAPHICS__COMMON__GL_COLOR_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// External Library
#include <GLES3.h>


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		struct GLByteRGB;


		// Color represented as HSL Values
		// Each values have range [0.0, 1.0]
		struct HSLColor
		{
		public:
			float hue;
			float saturation;
			float lightness;

		public:
			HSLColor()
				: HSLColor(0.0f, 0.0f, 0.0f)
			{ }

			HSLColor(float hue, float saturation, float lightness)
				: hue(hue), saturation(saturation), lightness(lightness)
			{ }

		public:
			static HSLColor FromRGB(float r, float g, float b);
			static HSLColor FromRGB(const GLByteRGB &rgb);

		public:
			GLByteRGB ToRGB() const;

			void HueShift(float amount);
			void SaturationAdjust(float amount);
			void SaturationMultiply(float amount);
			void LightnessAdjust(float amount);

			void SetHue(float value);
			void SetSaturation(float value);
			void SetLightness(float value);
		};


		// RGB Color by GLubyte
		struct GLByteRGB
		{
		public:
			GLubyte red;
			GLubyte green;
			GLubyte blue;

		public:
			GLByteRGB()
				: GLByteRGB(0, 0, 0)
			{ }

			GLByteRGB(GLubyte red, GLubyte green, GLubyte blue)
				: red(red), green(green), blue(blue)
			{ }

			GLByteRGB(int red, int green, int blue)
				: red((GLubyte)((red < 0) ? 0 : ((red > 255) ? 255 : red)))
				, green((GLubyte)((green < 0) ? 0 : ((green > 255) ? 255 : green)))
				, blue((GLubyte)((blue < 0) ? 0 : ((blue > 255) ? 255 : blue)))
			{ }

			GLByteRGB(float red, float green, float blue)
				: red((GLubyte)((red < 0.0f) ? 0.0f : ((red > 1.0f) ? 255.0f : (GLubyte)(red * 255.0f))))
				, green((GLubyte)((green < 0.0f) ? 0.0f : ((green > 1.0f) ? 255.0f : (GLubyte)(green * 255.0f))))
				, blue((GLubyte)((blue < 0.0f) ? 0.0f : ((blue > 1.0f) ? 255.0f : (GLubyte)(blue * 255.0f))))
			{ }

		public:
			static const GLByteRGB kBlack;
			static const GLByteRGB kWhite;
			static const GLByteRGB kRed;
			static const GLByteRGB kGreen;
			static const GLByteRGB kBlue;

		public:
			static GLByteRGB Grey(GLubyte brightness);
			static GLByteRGB Grey(int brightness);
			static GLByteRGB Grey(float brightness);

			// Create a color based on a Color value in range [0.0, 1.0]
			// Same as HSV Color with S=100%, V=100%, for [0 Degree, 360 Degree] mapped from [0.0, 1.0]
			static GLByteRGB ChromaColor(float chroma);

			static GLByteRGB FromHSL(float h, float s, float l);
			static GLByteRGB FromHSL(const HSLColor &hsl);

		public:
			HSLColor ToHSL() const;

		public:
			bool operator ==(const GLByteRGB &rhs) const
			{
				return ((red == rhs.red) && (green == rhs.green) && (blue == rhs.blue));
			}

			bool operator !=(const GLByteRGB &rhs) const
			{
				return !((*this) == rhs);
			}
		};


		// ARGB Color by GLubyte
		struct GLByteARGB
		{
		public:
			GLubyte alpha;
			GLubyte red;
			GLubyte green;
			GLubyte blue;

		public:
			using ColorCodeType = uint32_t;

		public:
			GLByteARGB()
				: GLByteARGB(0, 0, 0, 0)
			{ }

			GLByteARGB(GLubyte alpha, GLubyte red, GLubyte green, GLubyte blue)
				: alpha(alpha), red(red), green(green), blue(blue)
			{ }

			GLByteARGB(int alpha, int red, int green, int blue)
				: alpha((GLubyte)((alpha < 0) ? 0 : ((alpha > 255) ? 255 : alpha)))
				, red((GLubyte)((red < 0) ? 0 : ((red > 255) ? 255 : red)))
				, green((GLubyte)((green < 0) ? 0 : ((green > 255) ? 255 : green)))
				, blue((GLubyte)((blue < 0) ? 0 : ((blue > 255) ? 255 : blue)))
			{ }

			GLByteARGB(float alpha, float red, float green, float blue)
				: alpha((GLubyte)((alpha < 0.0f) ? 0.0f : ((alpha > 1.0f) ? 255.0f : (GLubyte)(alpha * 255.0f))))
				, red((GLubyte)((red < 0.0f) ? 0.0f : ((red > 1.0f) ? 255.0f : (GLubyte)(red * 255.0f))))
				, green((GLubyte)((green < 0.0f) ? 0.0f : ((green > 1.0f) ? 255.0f : (GLubyte)(green * 255.0f))))
				, blue((GLubyte)((blue < 0.0f) ? 0.0f : ((blue > 1.0f) ? 255.0f : (GLubyte)(blue * 255.0f))))
			{ }

			GLByteARGB(const GLByteRGB &color, GLubyte alpha = (GLubyte)255)
				: alpha(alpha), red(color.red), green(color.green), blue(color.blue)
			{ }

		public:
			ColorCodeType ToColorCodeARGB() const;
			ColorCodeType ToColorCodeRGBA() const;

			GLByteRGB ToGLByteRGB() const { return GLByteRGB(red, green, blue); }

		public:
			static const GLByteARGB kTransparent;
			static const GLByteARGB kBlack;
			static const GLByteARGB kWhite;
			static const GLByteARGB kRed;
			static const GLByteARGB kGreen;
			static const GLByteARGB kBlue;

		public:
			static GLByteARGB FromColorCodeARGB(ColorCodeType colorCode);
			static GLByteARGB FromColorCodeRGBA(ColorCodeType colorCode);

			static GLByteARGB Grey(GLubyte brightness, GLubyte alpha = (GLubyte)255);
			static GLByteARGB Grey(int brightness, int alpha = 255);
			static GLByteARGB Grey(float brightness, float alpha = 255.0f);

			// Create a color based on a Color value in range [0.0, 1.0]
			// Same as HSV Color with S=100%, V=100%, for [0 Degree, 360 Degree] mapped from [0.0, 1.0]
			static GLByteARGB ChromaColor(float chroma, float alpha = 255.0f);

		public:
			bool operator ==(const GLByteARGB &rhs) const
			{
				return ((red == rhs.red) && (green == rhs.green) && (blue == rhs.blue) && (alpha == rhs.alpha));
			}

			bool operator !=(const GLByteARGB &rhs) const
			{
				return !((*this) == rhs);
			}
		};
	}
}

#endif
