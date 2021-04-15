////////////////////////////////////////////////////////////////////////////////
// Texture/TextureSection.h (Leggiero/Modules - Graphics)
//
// Type to represent a subsection of a texture
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__TEXTURE_SECTION_H
#define __LM_GRAPHICS__TEXTURE__TEXTURE_SECTION_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class GLTextureResource;


		// Rectangular Sub-section of a Texture
		class TextureRectSection
		{
		public:
			TextureRectSection(int pixelLeft, int pixelTop, int pixelWidth, int pixelHeight, int texturePixelWidth, int texturePixelHeight, float scaleFactor = 1.0f)
				: m_pixelLeft(pixelLeft), m_pixelTop(pixelTop), m_pixelWidth(pixelWidth), m_pixelHeight(pixelHeight)
				, m_texelLeft((texturePixelWidth == 0) ? 0.0f : ((float)pixelLeft / (float)(texturePixelWidth * scaleFactor))), m_texelTop((texturePixelHeight == 0) ? 0.0f : ((float)pixelTop / (float)(texturePixelHeight * scaleFactor)))
				, m_texelRight((texturePixelWidth == 0) ? 0.0f : ((pixelWidth == 0) ? ((float)pixelLeft / (float)(texturePixelWidth * scaleFactor)) : ((float)(pixelLeft + pixelWidth) / (float)(texturePixelWidth * scaleFactor))))
				, m_texelBottom((texturePixelHeight == 0) ? 0.0f : ((pixelHeight == 0) ? ((float)pixelTop / (float)(texturePixelHeight * scaleFactor)) : ((float)(pixelTop + pixelHeight) / (float)(texturePixelHeight * scaleFactor))))
				, m_texelOnePixelWidth((texturePixelWidth == 0) ? 0.0f : (1.0f / (float)(texturePixelWidth * scaleFactor))), m_texelOnePixelHeight((texturePixelHeight == 0) ? 0.0f : (1.0f / (float)(texturePixelHeight * scaleFactor)))
			{ }

			TextureRectSection(int pixelLeft, int pixelTop, int pixelWidth, int pixelHeight, std::shared_ptr<GLTextureResource> texture, float scaleFactor = 1.0f);

			TextureRectSection()
				: TextureRectSection(0, 0, 0, 0, 0, 0)
			{ }

			virtual ~TextureRectSection() { }

		public:
			int GetPixelLeft() const { return m_pixelLeft; }
			int GetPixelTop() const { return m_pixelTop; }
			int GetPixelRight() const { return m_pixelLeft + m_pixelWidth; }
			int GetPixelBottom() const { return m_pixelTop + m_pixelHeight; }
			int GetPixelWidth() const { return m_pixelWidth; }
			int GetPixelHeight() const { return m_pixelHeight; }

			float GetTexelLeft() const { return m_texelLeft; }
			float GetTexelTop() const { return m_texelTop; }
			float GetTexelRight() const { return m_texelRight; }
			float GetTexelBottom() const { return m_texelBottom; }
			float GetTexelWidth() const { return m_texelRight - m_texelLeft; }
			float GetTexelHeight() const { return m_texelBottom - m_texelTop; }

			float GetTexelOnePixelWidth() const { return m_texelOnePixelWidth; }
			float GetTexelOnePixelHeight() const { return m_texelOnePixelHeight; }

		protected:
			int m_pixelLeft;
			int m_pixelTop;
			int m_pixelWidth;
			int m_pixelHeight;

			float m_texelLeft;
			float m_texelTop;
			float m_texelRight;
			float m_texelBottom;

			float m_texelOnePixelWidth;
			float m_texelOnePixelHeight;
		};
	}
}

#endif
