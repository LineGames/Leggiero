////////////////////////////////////////////////////////////////////////////////
// Texture/RenderTargetTextureResource.h (Leggiero/Modules - Graphics)
//
// Texture Resource can used as Render Target
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__RENDER_TARGET_TEXTURE_RESOURCE_H
#define __LM_GRAPHICS__TEXTURE__RENDER_TARGET_TEXTURE_RESOURCE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <tuple>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include "GLTextureResource.h"
#include "../Common/GLFrameBufferBindContext.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class RenderTargetTextureResource;


		// Render Target Texture Creation Functions
		std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureResource(GraphicResourceManagerComponent *ownerManager,
			GLsizei width, GLsizei height, bool isEnableDepth = false, bool isEnableStencil = false,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, bool isUsePower2Dimension = true);
		std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureResourceFromImageData(GraphicResourceManagerComponent *ownerManager,
			const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth = 0, GLsizei textureHeight = 0, bool isEnableDepth = false, bool isEnableStencil = false,
			GLuint minFilter = GL_NEAREST, GLuint magFilter = GL_NEAREST, GLuint wrapS = GL_CLAMP_TO_EDGE, GLuint wrapT = GL_CLAMP_TO_EDGE, bool isUsePower2Dimension = true);


		// Render Target Texture Resource
		class RenderTargetTextureResource
			: public GLTextureResource
		{
		// Only can be created by creation functions
		protected:
			friend std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureResource(GraphicResourceManagerComponent *, GLsizei, GLsizei, bool, bool, GLuint, GLuint, GLuint, GLuint, bool);
			friend std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureResourceFromImageData(GraphicResourceManagerComponent *, const void *, GLsizei, GLsizei, GLsizei, GLsizei, bool, bool, GLuint, GLuint, GLuint, GLuint, bool);

			RenderTargetTextureResource(
				GraphicResourceManagerComponent *ownerManager,
				Texture::GLESRawTextureInformation textureInfo, GLuint frameBufferName, GLuint depthRenderBufferName, GLuint stencilRenderBufferName,
				GLsizei width, GLsizei height, bool isEnableDepth, bool isEnableStencil,
				GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, bool isUsePower2Dimension);

		public:
			virtual ~RenderTargetTextureResource();

		// IGLGraphicResource
		public:
			virtual bool IsValid() override;
			virtual bool Restore() override;

		public:
			virtual bool RestoreWithImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight);

		public:
			virtual std::tuple<bool, GLFrameBufferBindContext> BindToFrameBufferTarget(bool isAutoUnBind = false);

			bool IsFreshRestored() const { return m_isRestored; }
			void SetFreshRestoreFlag(bool restoredState) { m_isRestored = restoredState; }

			GLsizei GetRequiredWidth() const { return m_width; }
			GLsizei GetRequiredHeight() const { return m_height; }

			bool IsDepthEnabled() const { return m_isEnableDepth; }
			bool IsStencilEnabled() const { return m_isEnableStencil; }

		protected:
			virtual std::function<void()> _GenerateResourceTrashFunc() override;

		protected:
			GLsizei m_width;
			GLsizei m_height;
			bool m_isEnableDepth;
			bool m_isEnableStencil;

			bool m_isUsePower2Dimension;

			bool m_isRestored;

		protected:
			GLuint m_frameBufferName;
			GLuint m_depthRenderBufferName;
			GLuint m_stencilRenderBufferName;
		};
	}
}

#endif
