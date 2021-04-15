////////////////////////////////////////////////////////////////////////////////
// Texture/TextureManager.h (Leggiero/Modules - Graphics)
//
// Texture Resource Manager Class (member of Resource Manager Component)
// All Load and Creation Function should be called in the thread w/ GL Context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__TEXTURE_MANAGER_H
#define __LM_GRAPHICS__TEXTURE__TEXTURE_MANAGER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include "ImageFormatType.h"
#include "DynamicTextureRestore.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class GraphicResourceManagerComponent;
		class GLTextureResource;
		class DynamicTextureResource;
		class RenderTargetTextureResource;


		// Texture Manager
		class TextureManager
			: private Utility::SyntacticSugar::NonCopyable
		{
		public:
			TextureManager(GraphicResourceManagerComponent *ownerManager);
			virtual ~TextureManager();

		public:
			std::shared_ptr<GLTextureResource> LoadTextureFromImageInMemory(const void *data, size_t size, ImageFormatType imageType, bool isPersistenceMemory = false,
				GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = false, bool isCommitTexture = true);
			std::shared_ptr<GLTextureResource> LoadTextureFromImageInFile(std::string filePath, ImageFormatType imageType = ImageFormatType::kINVALID,
				GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = false, bool isCommitTexture = true);
			std::shared_ptr<GLTextureResource> LoadTextureFromImageInBundle(std::string virtualPath, ImageFormatType imageType = ImageFormatType::kINVALID,
				GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = false, bool isCommitTexture = true);

		public:
			std::shared_ptr<DynamicTextureResource> CreateDynamicTexture(GLsizei width, GLsizei height, DynamicTextureRestoreNotifyDelegate restoreNotifier = PredefinedDynamicTextureNotifier::kNonNotifyingDynamicTextureRestoreNotifier,
				GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = true);
			std::shared_ptr<DynamicTextureResource> CreateDynamicTextureFromImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth = 0, GLsizei textureHeight = 0, DynamicTextureRestoreNotifyDelegate restoreNotifier = PredefinedDynamicTextureNotifier::kNonNotifyingDynamicTextureRestoreNotifier,
				GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = true, bool isCommitTexture = true);

		public:
			std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTexture(GLsizei width, GLsizei height, bool isEnableDepth = false, bool isEnableStencil = false,
				GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, bool isUsePower2Dimension = true);
			std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureFromImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth = 0, GLsizei textureHeight = 0, bool isEnableDepth = false, bool isEnableStencil = false,
				GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, bool isUsePower2Dimension = true, bool isCommitTexture = true);

		protected:
			GraphicResourceManagerComponent *m_ownerResourceManager;
		};
	}
}

#endif
