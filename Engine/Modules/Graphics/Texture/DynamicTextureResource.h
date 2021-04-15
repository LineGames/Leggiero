////////////////////////////////////////////////////////////////////////////////
// Texture/DynamicTextureResource.h (Leggiero/Modules - Graphics)
//
// Texture Resource with Dynamically Generated Contents
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__DYNAMIC_TEXTURE_RESOURCE_H
#define __LM_GRAPHICS__TEXTURE__DYNAMIC_TEXTURE_RESOURCE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include "DynamicTextureRestore.h"
#include "GLTextureResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Dynamic Texture Creation Functions
		std::shared_ptr<DynamicTextureResource> CreateDynamicTextureResource(GraphicResourceManagerComponent *ownerManager,
			GLsizei width, GLsizei height, DynamicTextureRestoreNotifyDelegate restoreNotifier = PredefinedDynamicTextureNotifier::kNonNotifyingDynamicTextureRestoreNotifier,
			GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = true);
		std::shared_ptr<DynamicTextureResource> CreateDynamicTextureResourceFromImageData(GraphicResourceManagerComponent *ownerManager,
			const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth = 0, GLsizei textureHeight = 0, DynamicTextureRestoreNotifyDelegate restoreNotifier = PredefinedDynamicTextureNotifier::kNonNotifyingDynamicTextureRestoreNotifier,
			GLenum format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, GLint mipmapLevel = 0, bool isUsePower2Dimension = true);


		// Dynamic Texture Resource
		class DynamicTextureResource
			: public GLTextureResource
			, public std::enable_shared_from_this<DynamicTextureResource>
		{
		// Only can be created by creation functions
		protected:
			friend std::shared_ptr<DynamicTextureResource> CreateDynamicTextureResource(GraphicResourceManagerComponent *, GLsizei, GLsizei, DynamicTextureRestoreNotifyDelegate, GLenum, GLenum, GLuint, GLuint, GLuint, GLuint, GLint, bool);
			friend std::shared_ptr<DynamicTextureResource> CreateDynamicTextureResourceFromImageData(GraphicResourceManagerComponent *, const void *, GLsizei, GLsizei, GLsizei, GLsizei, DynamicTextureRestoreNotifyDelegate, GLenum, GLenum, GLuint, GLuint, GLuint, GLuint, GLint, bool);

			DynamicTextureResource(
				GraphicResourceManagerComponent *ownerManager,
				Texture::GLESRawTextureInformation textureInfo,
				GLsizei width, GLsizei height,
				DynamicTextureRestoreNotifyDelegate restoreNotifier,
				GLenum format, GLenum dataType,
				GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension);

		public:
			virtual ~DynamicTextureResource();

		// IGLGraphicResource
		public:
			virtual bool Restore() override;

		public:
			virtual bool RestoreSlient();
			virtual bool RestoreWithImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight, bool isSuppressRestoreNotify = true);

		public:
			virtual void UploadSubImage(GLint x, GLint y, const void *imageData, GLsizei imageWidth, GLsizei imageHeight);
			virtual void CopyFromFrameBuffer(GLint sourceX, GLint sourceY, GLint x, GLint y, GLsizei imageWidth, GLsizei imageHeight);

		public:
			void ChangeNotifier(DynamicTextureRestoreNotifyDelegate newNotifer) { m_restoreNotifier = newNotifer; }
			void UnSubscribeNotifier() { ChangeNotifier(PredefinedDynamicTextureNotifier::kNonNotifyingDynamicTextureRestoreNotifier); }

		public:
			GLsizei GetRequiredWidth() const { return m_width; }
			GLsizei GetRequiredHeight() const { return m_height; }

			GLenum GetImageFormat() const { return m_format; }
			GLenum GetImageDataType() const { return m_dataType; }

			GLint GetMipmapLevel() const { return m_mipmapLevel; }
			bool IsUsePower2DDimension() const { return m_isUsePower2Dimension; }

		protected:
			GLsizei m_width;
			GLsizei m_height;

			GLenum m_format;
			GLenum m_dataType;

			GLint m_mipmapLevel;
			bool m_isUsePower2Dimension;

			DynamicTextureRestoreNotifyDelegate m_restoreNotifier;
		};
	}
}

#endif
