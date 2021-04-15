////////////////////////////////////////////////////////////////////////////////
// Texture/TextureHelper.h (Leggiero/Modules - Graphics)
//
// Helper Functions related to Texture
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__TEXTURE_HELPER_H
#define __LM_GRAPHICS__TEXTURE__TEXTURE_HELPER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <iostream>
#include <string>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include "ImageFormatType.h"


namespace Leggiero
{
	namespace Graphics
	{
		namespace Texture
		{
			// Texture name represents Invalid Texture
			constexpr GLuint kINVALID_TEXTURE_NAME = 0xFFFFFFFF;


			// Struct to store texture created from OpenGL ES
			struct GLESRawTextureInformation
			{
			public:	// Texture Information
				GLuint	name;
				GLenum	target;
				GLsizei width;
				GLsizei height;
				GLsizei contentWidth;
				GLsizei contentHeight;
				bool	hasAlpha;
				bool	hasMimmap;

			public:	// Interface
				bool operator==(const GLESRawTextureInformation &b) const;
				bool IsValid() const { return (name != kINVALID_TEXTURE_NAME); }

			public:	// Constants
				const static GLESRawTextureInformation kINVALID;
			};


			GLsizei GetTexelByteSize(GLenum format, GLenum dataType);
			bool HasFormatAlpha(GLenum format);


			namespace Loader
			{
				// Texture Loader
				GLESRawTextureInformation LoadTextureFromImageInMemory(const void *data, size_t size, ImageFormatType imageType, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
				GLESRawTextureInformation LoadTextureFromImageInStream(std::istream &sourceStream, ImageFormatType imageType, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
				GLESRawTextureInformation LoadTextureFromImageInFile(const char *filePath, ImageFormatType imageType = ImageFormatType::kINVALID, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);

				// Raw Texture
				GLESRawTextureInformation LoadTextureFromBitmapDataInMemory(const void *data,
					GLsizei width, GLsizei height,
					GLenum format, GLenum dataType,
					GLint mipmapLevel = 0,
					bool isUsePower2Dimension = false);

				namespace PNG
				{
					// PNG Loaders
					GLESRawTextureInformation LoadPNGTextureFromMemory(const void *data, size_t size, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
					GLESRawTextureInformation LoadPNGTextureFromStream(std::istream &sourceStream, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
					GLESRawTextureInformation LoadPNGTextureFromFile(const char *filePath, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
				}

				namespace JPEG
				{
					// JPEG Loaders
					GLESRawTextureInformation LoadJPEGTextureFromMemory(const void *data, size_t size, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
					GLESRawTextureInformation LoadJPEGTextureFromStream(std::istream &sourceStream, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
					GLESRawTextureInformation LoadJPEGTextureFromFile(const char *filePath, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
				}
			}
		}
	}
}

#endif
