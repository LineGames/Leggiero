////////////////////////////////////////////////////////////////////////////////
// Texture/GLTextureResource_Creation.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Creation Functions of OpenGL ES Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLTextureResource.h"

// Standard Library
#include <sstream>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Graphics
#include "../_Internal/_DummyInterfaces.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Texture Creation Functions

		//------------------------------------------------------------------------------
		// Create Texture Resource from data in memory
		std::shared_ptr<GLTextureResource> LoadTextureResourceFromImageInMemory(GraphicResourceManagerComponent *ownerManager, const void *data, size_t size, ImageFormatType imageType, bool isPersistenceMemory,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
		{
			Texture::GLESRawTextureInformation creatingTextureInfo = Texture::Loader::LoadTextureFromImageInMemory(data, size, imageType, mipmapLevel, isUsePower2Dimension);
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

			GLTextureResource *createdResource = nullptr;
			if (isPersistenceMemory)
			{
				createdResource = new GLTextureResource(ownerManager, creatingTextureInfo,
					[data, size, imageType, mipmapLevel, isUsePower2Dimension]() { return Texture::Loader::LoadTextureFromImageInMemory(data, size, imageType, mipmapLevel, isUsePower2Dimension); },
					minFilter, magFilter, wrapS, wrapT);
			}
			else
			{
				createdResource = new GLTextureResource(ownerManager, creatingTextureInfo,
					std::bind(_Internal::_GLTextureResourceCreatorFunc_Invalid),
					minFilter, magFilter, wrapS, wrapT);
			}

			return std::shared_ptr<GLTextureResource>(createdResource);
		}

		//------------------------------------------------------------------------------
		// Create Texture Resource from file
		std::shared_ptr<GLTextureResource> LoadTextureResourceFromImageInFile(GraphicResourceManagerComponent *ownerManager, const std::string &filePath, ImageFormatType imageType,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
		{
			Texture::GLESRawTextureInformation creatingTextureInfo = Texture::Loader::LoadTextureFromImageInFile(filePath.c_str(), imageType, mipmapLevel, isUsePower2Dimension);
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

			GLTextureResource *createdResource = new GLTextureResource(ownerManager, creatingTextureInfo,
				[filePath, imageType, mipmapLevel, isUsePower2Dimension]() { return Texture::Loader::LoadTextureFromImageInFile(filePath.c_str(), imageType, mipmapLevel, isUsePower2Dimension); },
				minFilter, magFilter, wrapS, wrapT);

			return std::shared_ptr<GLTextureResource>(createdResource);
		}

		//------------------------------------------------------------------------------
		// Create Texture Resource from resource in bundle
		std::shared_ptr<GLTextureResource> LoadTextureResourceFromImageInBundle(GraphicResourceManagerComponent *ownerManager, FileSystem::BundleFileResourceComponent *bundle, const std::string &virtualPath, ImageFormatType imageType,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
		{
			// Real File Access Optimization
			if (bundle->NPO_IsEnable_BundleFileRealPath())
			{
				return LoadTextureResourceFromImageInFile(ownerManager, bundle->NPO_GetBundleFileRealPath(virtualPath), imageType, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
			}

			// Load Resource
			std::stringstream textureData;
			bundle->ReadBundleFileData(virtualPath, 0, textureData);
			textureData.seekg(0);

			Texture::GLESRawTextureInformation creatingTextureInfo = Texture::Loader::LoadTextureFromImageInStream(textureData, imageType, mipmapLevel, isUsePower2Dimension);
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

			GLTextureResource *createdResource = new GLTextureResource(ownerManager, creatingTextureInfo,
				[bundle, virtualPath, imageType, mipmapLevel, isUsePower2Dimension]() {
					std::stringstream textureData;
					bundle->ReadBundleFileData(virtualPath, 0, textureData);
					textureData.seekg(0);
					return Texture::Loader::LoadTextureFromImageInStream(textureData, imageType, mipmapLevel, isUsePower2Dimension);
				},
				minFilter, magFilter, wrapS, wrapT);

			return std::shared_ptr<GLTextureResource>(createdResource);
		}


		//////////////////////////////////////////////////////////////////////////////// Dummy Texture Creation Functions

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			// Creator Function for non-restorable resources
			Texture::GLESRawTextureInformation _GLTextureResourceCreatorFunc_Invalid()
			{
				return Texture::GLESRawTextureInformation::kINVALID;
			}

			//------------------------------------------------------------------------------
			// Dummy Creator Function
			Texture::GLESRawTextureInformation _GLTextureResourceCreatorFunc_Dummy()
			{
				return Texture::GLESRawTextureInformation::kINVALID;
			}
		}
	}
}
