////////////////////////////////////////////////////////////////////////////////
// Texture/TextureManager.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Texture Manager
// This is for divide implementation of GraphicResourceManagerComponent to
// to avoid to be gigantic monolithic manager component.
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureManager.h"

// Leggiero.Graphics
#include "../GraphicResourceManagerComponent.h"
#include "GLTextureResource.h"
#include "DynamicTextureResource.h"
#include "RenderTargetTextureResource.h"
#include "TextureCommitter.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// TextureManager

		//------------------------------------------------------------------------------
		TextureManager::TextureManager(GraphicResourceManagerComponent *ownerManager)
			: m_ownerResourceManager(ownerManager)
		{
		}

		//------------------------------------------------------------------------------
		TextureManager::~TextureManager()
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLTextureResource> TextureManager::LoadTextureFromImageInMemory(const void *data, size_t size, ImageFormatType imageType, bool isPersistenceMemory, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension, bool isCommitTexture)
		{
			std::shared_ptr<GLTextureResource> createdResource = LoadTextureResourceFromImageInMemory(m_ownerResourceManager, data, size, imageType, isPersistenceMemory, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
			if (createdResource)
			{
				if (isCommitTexture)
				{
					m_ownerResourceManager->GetTextureCommitter().CommitTexture(createdResource);
				}

				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLTextureResource> TextureManager::LoadTextureFromImageInFile(std::string filePath, ImageFormatType imageType, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension, bool isCommitTexture)
		{
			std::shared_ptr<GLTextureResource> createdResource = LoadTextureResourceFromImageInFile(m_ownerResourceManager, filePath, imageType, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
			if (createdResource)
			{
				if (isCommitTexture)
				{
					m_ownerResourceManager->GetTextureCommitter().CommitTexture(createdResource);
				}

				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLTextureResource> TextureManager::LoadTextureFromImageInBundle(std::string virtualPath, ImageFormatType imageType, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension, bool isCommitTexture)
		{
			if (m_ownerResourceManager->m_bundleFileSystemComponent == nullptr)
			{
				// Just fail when bundle file system not exists
				return nullptr;
			}

			ImageFormatType loadingImageType = imageType;
			if (imageType == ImageFormatType::kINVALID)
			{
				// Detect Image Type using Virtual Path's Extension
				loadingImageType = GetImageFormatFromFilePathExtension(virtualPath.c_str());
			}

			std::shared_ptr<GLTextureResource> createdResource = LoadTextureResourceFromImageInBundle(m_ownerResourceManager, m_ownerResourceManager->m_bundleFileSystemComponent, virtualPath, loadingImageType, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
			if (createdResource)
			{
				if (isCommitTexture)
				{
					m_ownerResourceManager->GetTextureCommitter().CommitTexture(createdResource);
				}

				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<DynamicTextureResource> TextureManager::CreateDynamicTexture(GLsizei width, GLsizei height, DynamicTextureRestoreNotifyDelegate restoreNotifier, GLenum format, GLenum dataType, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
		{
			std::shared_ptr<DynamicTextureResource> createdResource = CreateDynamicTextureResource(m_ownerResourceManager, width, height, restoreNotifier,
				format, dataType, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
			if (createdResource)
			{
				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<DynamicTextureResource> TextureManager::CreateDynamicTextureFromImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth, GLsizei textureHeight, DynamicTextureRestoreNotifyDelegate restoreNotifier, GLenum format, GLenum dataType, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension, bool isCommitTexture)
		{
			std::shared_ptr<DynamicTextureResource> createdResource = CreateDynamicTextureResourceFromImageData(m_ownerResourceManager, imageData, imageWidth, imageHeight, textureWidth, textureHeight, restoreNotifier,
				format, dataType, minFilter, magFilter, wrapS, wrapT, mipmapLevel, isUsePower2Dimension);
			if (createdResource)
			{
				if (isCommitTexture)
				{
					m_ownerResourceManager->GetTextureCommitter().CommitTexture(createdResource, imageWidth, imageHeight);
				}

				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<RenderTargetTextureResource> TextureManager::CreateRenderTargetTexture(GLsizei width, GLsizei height, bool isEnableDepth, bool isEnableStencil, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, bool isUsePower2Dimension)
		{
			std::shared_ptr<RenderTargetTextureResource> createdResource = CreateRenderTargetTextureResource(m_ownerResourceManager, width, height, isEnableDepth, isEnableStencil, minFilter, magFilter, wrapS, wrapT, isUsePower2Dimension);
			if (createdResource)
			{
				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<RenderTargetTextureResource> TextureManager::CreateRenderTargetTextureFromImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth, GLsizei textureHeight, bool isEnableDepth, bool isEnableStencil, GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, bool isUsePower2Dimension, bool isCommitTexture)
		{
			std::shared_ptr<RenderTargetTextureResource> createdResource = CreateRenderTargetTextureResourceFromImageData(m_ownerResourceManager, imageData, imageWidth, imageHeight, textureWidth, textureHeight, isEnableDepth, isEnableStencil, minFilter, magFilter, wrapS, wrapT, isUsePower2Dimension);
			if (createdResource)
			{
				if (isCommitTexture)
				{
					m_ownerResourceManager->GetTextureCommitter().CommitTexture(createdResource, imageWidth, imageHeight);
				}

				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}
	}
}
