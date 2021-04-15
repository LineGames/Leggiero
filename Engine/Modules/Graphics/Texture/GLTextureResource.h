////////////////////////////////////////////////////////////////////////////////
// Texture/GLTextureResource.h (Leggiero/Modules - Graphics)
//
// OpenGL Texture Resource
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__GL_TEXTURE_RESOURCE_H
#define __LM_GRAPHICS__TEXTURE__GL_TEXTURE_RESOURCE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include "../Common/IGLGraphicResource.h"
#include "ImageFormatType.h"
#include "TextureHelper.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Utility
	{
		namespace Object
		{
			class PointerHolder;
		}
	}


	// Forward Declaration of File System Component
	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}


	namespace Graphics
	{
		// Forward Declarations
		class GraphicResourceManagerComponent;
		class GLTextureResource;


		// Texture Creation Functions
		std::shared_ptr<GLTextureResource> LoadTextureResourceFromImageInMemory(GraphicResourceManagerComponent *ownerManager, const void *data, size_t size, ImageFormatType imageType, bool isPersistenceMemory = false,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
		std::shared_ptr<GLTextureResource> LoadTextureResourceFromImageInFile(GraphicResourceManagerComponent *ownerManager, const std::string &filePath, ImageFormatType imageType = ImageFormatType::kINVALID,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);
		std::shared_ptr<GLTextureResource> LoadTextureResourceFromImageInBundle(GraphicResourceManagerComponent *ownerManager, FileSystem::BundleFileResourceComponent *bundle, const std::string &virtualPath, ImageFormatType imageType,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = false);


		// OpenGL ES Texture Resource
		class GLTextureResource
			: public IGLGraphicResource
			, private Utility::SyntacticSugar::NonCopyable
		{
		// Only can be created by creation functions
		protected:
			friend std::shared_ptr<GLTextureResource> Graphics::LoadTextureResourceFromImageInMemory(GraphicResourceManagerComponent *, const void *, size_t, ImageFormatType, bool, GLuint, GLuint, GLuint, GLuint, GLint, bool);
			friend std::shared_ptr<GLTextureResource> Graphics::LoadTextureResourceFromImageInFile(GraphicResourceManagerComponent *, const std::string &, ImageFormatType, GLuint, GLuint, GLuint, GLuint, GLint, bool);
			friend std::shared_ptr<GLTextureResource> Graphics::LoadTextureResourceFromImageInBundle(GraphicResourceManagerComponent *, FileSystem::BundleFileResourceComponent *, const std::string &, ImageFormatType, GLuint, GLuint, GLuint, GLuint, GLint, bool);

			GLTextureResource(
				GraphicResourceManagerComponent *ownerManager,
				Texture::GLESRawTextureInformation textureInfo, std::function<Texture::GLESRawTextureInformation()> textureCreator,
				GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT);

		public:
			virtual ~GLTextureResource();

		public:
			Texture::GLESRawTextureInformation GetTextureInfo() const { return m_textureInfo; }

		public:
			// Bind the texture resource to given texture unit in OpenGL ES context
			bool Bind(GLenum textureUnit = GL_TEXTURE0);

			GLuint GetMinFilter() const { return m_minFilter; }
			GLuint GetMagFilter() const { return m_magFilter; }
			void SetTextureFilter(GLuint minFilter, GLuint magFilter);

			GLuint GetWrapS() const { return m_wrapS; }
			GLuint GetWrapT() const { return m_wrapT; }
			void SetWarp(GLuint wrapS, GLuint wrapT);

		public:	// IGLGraphicResource
			virtual bool IsValid() override;
			virtual bool Restore() override;

		protected:
			virtual std::function<void()> _GenerateResourceTrashFunc();

		protected:
			bool m_isInvalidated;
			Texture::GLESRawTextureInformation m_textureInfo;

			GLuint m_minFilter;
			GLuint m_magFilter;
			GLuint m_wrapS;
			GLuint m_wrapT;

			std::weak_ptr<Utility::Object::PointerHolder> m_managerHolder;

			std::function<Texture::GLESRawTextureInformation()> m_textureCreator;
		};
	}
}

#endif
