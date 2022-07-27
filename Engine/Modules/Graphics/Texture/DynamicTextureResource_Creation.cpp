////////////////////////////////////////////////////////////////////////////////
// Texture/DynamicTextureResource_Creation.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Creation Functions of Dynamic Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "DynamicTextureResource.h"

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Texture Creation Functions

		//------------------------------------------------------------------------------
		// Create a Dynamic Texture Resource
		std::shared_ptr<DynamicTextureResource> CreateDynamicTextureResource(GraphicResourceManagerComponent *ownerManager,
			GLsizei width, GLsizei height, DynamicTextureRestoreNotifyDelegate restoreNotifier, GLenum format, GLenum dataType,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
		{
			return CreateDynamicTextureResourceFromImageData(ownerManager, nullptr, 0, 0, width, height, restoreNotifier, format, dataType, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
		}

		//------------------------------------------------------------------------------
		// Create a Dynamic Texture Resource with Given Image Data
		std::shared_ptr<DynamicTextureResource> CreateDynamicTextureResourceFromImageData(GraphicResourceManagerComponent *ownerManager,
			const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth, GLsizei textureHeight, DynamicTextureRestoreNotifyDelegate restoreNotifier, GLenum format, GLenum dataType,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
		{
			GLsizei effectiveWidth = textureWidth;
			GLsizei effectiveHeight = textureHeight;
			if (textureWidth == 0)
			{
				effectiveWidth = imageWidth;
				textureWidth = imageWidth;
			}
			if (textureHeight == 0)
			{
				effectiveHeight = imageHeight;
				textureHeight = imageHeight;
			}
			if (isUsePower2Dimension)
			{
				effectiveWidth = Utility::Math::SmallestGreaterEqualPow2(effectiveWidth);
				effectiveHeight = Utility::Math::SmallestGreaterEqualPow2(effectiveHeight);
			}

			const void *creatingData = imageData;
			if (creatingData != nullptr)
			{
				if (imageWidth != textureWidth || imageHeight != textureHeight)
				{
					creatingData = nullptr;
				}
			}

			Texture::GLESRawTextureInformation creatingTextureInfo = Texture::Loader::LoadTextureFromBitmapDataInMemory(creatingData, textureWidth, textureHeight, format, dataType, mipmapLevel, isUsePower2Dimension);

			if (!creatingTextureInfo.IsValid())
			{
				// Texture Creation Failed
				return nullptr;
			}

			glBindTexture(GL_TEXTURE_2D, creatingTextureInfo.name);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
			if (creatingData == nullptr && imageData != nullptr)
			{
				glTexSubImage2D(GL_TEXTURE_2D, mipmapLevel, 0, 0, imageWidth, imageHeight, format, dataType, imageData);
                #ifdef _LEGGIERO_IOS
                    glFlush();
                #else
                    glFinish();
                #endif
			}

			return std::shared_ptr<DynamicTextureResource>(new DynamicTextureResource(ownerManager,
				creatingTextureInfo, textureWidth, textureHeight,
				restoreNotifier, format, dataType,
				minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension));
		}
	}
}
