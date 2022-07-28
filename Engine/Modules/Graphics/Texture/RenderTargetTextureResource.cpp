////////////////////////////////////////////////////////////////////////////////
// Texture/RenderTargetTextureResource.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Render Target Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "RenderTargetTextureResource.h"

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>

// Leggiero.Graphics
#include "../_Internal/_DummyInterfaces.h"
#include "TextureHelper.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// RenderTargetTextureResource

		//------------------------------------------------------------------------------
		RenderTargetTextureResource::RenderTargetTextureResource(GraphicResourceManagerComponent *ownerManager,
			Texture::GLESRawTextureInformation textureInfo, GLuint frameBufferName, GLuint depthRenderBufferName, GLuint stencilRenderBufferName,
			GLsizei width, GLsizei height, bool isEnableDepth, bool isEnableStencil,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, bool isUsePower2Dimension)
			: GLTextureResource(ownerManager, textureInfo, std::bind(_Internal::_GLTextureResourceCreatorFunc_Dummy), minFilter, magFilter, wrapS, wrapT)
			, m_frameBufferName(frameBufferName), m_depthRenderBufferName(depthRenderBufferName), m_stencilRenderBufferName(stencilRenderBufferName)
			, m_width(width), m_height(height), m_isEnableDepth(isEnableDepth), m_isEnableStencil(isEnableStencil)
			, m_isUsePower2Dimension(isUsePower2Dimension)
			, m_isRestored(false)
		{
		}

		//------------------------------------------------------------------------------
		RenderTargetTextureResource::~RenderTargetTextureResource()
		{
		}

		//------------------------------------------------------------------------------
		// Check validity of the texture resource.
		// Should be called in graphic thread with OpenGL context.
		bool RenderTargetTextureResource::IsValid()
		{
			if (m_isInvalidated)
			{
				return false;
			}

			if (m_textureInfo.IsValid() && (glIsTexture(m_textureInfo.name) == GL_TRUE) && (glIsFramebuffer(m_frameBufferName) == GL_TRUE))
			{
				return true;
			}

			// Invalidate All
			m_isInvalidated = true;
			if (m_textureInfo.IsValid() && (glIsTexture(m_textureInfo.name) == GL_TRUE))
			{
				glDeleteTextures(1, &(m_textureInfo.name));
			}
			if (glIsFramebuffer(m_frameBufferName) == GL_TRUE)
			{
				glDeleteFramebuffers(1, &m_frameBufferName);
			}
			if (m_isEnableDepth && (glIsRenderbuffer(m_depthRenderBufferName) == GL_TRUE))
			{
				glDeleteRenderbuffers(1, &m_depthRenderBufferName);
			}
			if (m_isEnableStencil && (glIsRenderbuffer(m_stencilRenderBufferName) == GL_TRUE))
			{
				glDeleteRenderbuffers(1, &m_stencilRenderBufferName);
			}

			return false;
		}

		//------------------------------------------------------------------------------
		// Restore invalidated texture resource.
		// Should be called in graphic thread with OpenGL context.
		bool RenderTargetTextureResource::Restore()
		{
			if (!m_isInvalidated)
			{
				if (IsValid())
				{
					// Not Need to Restore
					return true;
				}
			}

			return RestoreWithImageData(nullptr, 0, 0);
		}

		//------------------------------------------------------------------------------
		// Restore invalidated texture resource with given image data (of RGBA unsigned byte format).
		// Should be called in graphic thread with OpenGL context.
		bool RenderTargetTextureResource::RestoreWithImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight)
		{
			if (!m_isInvalidated)
			{
				if (IsValid())
				{
					// Not Need to Restore
					return true;
				}
			}

			GLsizei effectiveWidth = m_width;
			GLsizei effectiveHeight = m_height;
			if (m_isUsePower2Dimension)
			{
				effectiveWidth = Utility::Math::SmallestGreaterEqualPow2(effectiveWidth);
				effectiveHeight = Utility::Math::SmallestGreaterEqualPow2(effectiveHeight);
			}

			const void *creatingData = imageData;
			if (creatingData != nullptr)
			{
				if (imageWidth != m_width || imageHeight != m_height)
				{
					creatingData = nullptr;
				}
			}

			m_textureInfo = Texture::Loader::LoadTextureFromBitmapDataInMemory(creatingData, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0, m_isUsePower2Dimension);
			if (!m_textureInfo.IsValid())
			{
				return false;
			}

			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
			if (creatingData == nullptr && imageData != nullptr)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
                #ifdef _LEGGIERO_IOS
                    glFlush();
                #else
                    glFinish();
                #endif
			}

			GLint prevBinded;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBinded);

			GLint prevViewport[4];
			glGetIntegerv(GL_VIEWPORT, prevViewport);

			GLFrameBufferBindContext frameBufferRevertContext((GLuint)prevBinded, (GLint)prevViewport[0], (GLint)prevViewport[1], (GLsizei)prevViewport[2], (GLsizei)prevViewport[3], true);

			glGenFramebuffers(1, &m_frameBufferName);
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferName);

			glGetError();

			if (m_isEnableDepth)
			{
				glGenRenderbuffers(1, &m_depthRenderBufferName);
				glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBufferName);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, effectiveWidth, effectiveHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBufferName);

				if (glGetError() != GL_NO_ERROR)
				{
					glDeleteFramebuffers(1, &m_frameBufferName);
					glDeleteTextures(1, &(m_textureInfo.name));
					return false;
				}
			}

			if (m_isEnableStencil)
			{
				glGenRenderbuffers(1, &m_stencilRenderBufferName);
				glBindRenderbuffer(GL_RENDERBUFFER, m_stencilRenderBufferName);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, effectiveWidth, effectiveHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_stencilRenderBufferName);

				if (glGetError() != GL_NO_ERROR)
				{
					glDeleteFramebuffers(1, &m_frameBufferName);
					if (m_isEnableDepth)
					{
						glDeleteRenderbuffers(1, &m_depthRenderBufferName);
					}
					glDeleteTextures(1, &(m_textureInfo.name));
					return false;
				}
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureInfo.name, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				glDeleteFramebuffers(1, &m_frameBufferName);
				if (m_isEnableDepth)
				{
					glDeleteRenderbuffers(1, &m_depthRenderBufferName);
				}
				if (m_isEnableStencil)
				{
					glDeleteRenderbuffers(1, &m_stencilRenderBufferName);
				}
				glDeleteTextures(1, &(m_textureInfo.name));
				return false;
			}

			m_isInvalidated = false;
			m_isRestored = true;

			return true;
		}

		//------------------------------------------------------------------------------
		// Bind the texture to rendering target frame buffer
		std::tuple<bool, GLFrameBufferBindContext> RenderTargetTextureResource::BindToFrameBufferTarget(bool isAutoUnBind)
		{
			if (!m_textureInfo.IsValid())
			{
				return std::make_tuple(false, GLFrameBufferBindContext());
			}
			if (glIsFramebuffer(m_frameBufferName) == GL_FALSE)
			{
				return std::make_tuple(false, GLFrameBufferBindContext());
			}

			GLint prevBinded;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBinded);

			GLint prevViewport[4];
			glGetIntegerv(GL_VIEWPORT, prevViewport);

			glGetError();

			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferName);

			if (glGetError() != GL_NO_ERROR)
			{
				return std::make_tuple(false, GLFrameBufferBindContext());
			}

			glViewport(0, 0, m_width, m_height);

			return std::make_tuple(true, GLFrameBufferBindContext((GLuint)prevBinded,
				(GLint)prevViewport[0], (GLint)prevViewport[1], (GLsizei)prevViewport[2], (GLsizei)prevViewport[3],
				isAutoUnBind));
		}

		//------------------------------------------------------------------------------
		// Create function object to finalize the resource
		std::function<void()> RenderTargetTextureResource::_GenerateResourceTrashFunc()
		{
			bool isTextureValid = m_textureInfo.IsValid();
			GLuint textureName = m_textureInfo.name;
			GLuint frameBufferName = m_frameBufferName;
			bool isUseDepthBuffer = m_isEnableDepth;
			GLuint depthRenderBufferName = m_depthRenderBufferName;
			bool isUseStencilBuffer = m_isEnableStencil;
			GLuint stencilRenderBufferName = m_stencilRenderBufferName;

			return [isTextureValid, textureName, frameBufferName, isUseDepthBuffer, depthRenderBufferName, isUseStencilBuffer, stencilRenderBufferName]() mutable {
				if (glIsFramebuffer(frameBufferName) == GL_TRUE)
				{
					glDeleteFramebuffers(1, &frameBufferName);
				}
				if (isUseDepthBuffer)
				{
					if (glIsRenderbuffer(depthRenderBufferName) == GL_TRUE)
					{
						glDeleteRenderbuffers(1, &depthRenderBufferName);
					}
				}
				if (isUseStencilBuffer)
				{
					if (glIsRenderbuffer(stencilRenderBufferName) == GL_TRUE)
					{
						glDeleteRenderbuffers(1, &stencilRenderBufferName);
					}
				}
				if (isTextureValid)
				{
					if (glIsTexture(textureName) == GL_TRUE)
					{
						glDeleteTextures(1, &textureName);
					}
				}
			};
		}
	}
}
