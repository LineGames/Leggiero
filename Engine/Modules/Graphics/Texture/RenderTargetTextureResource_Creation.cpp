////////////////////////////////////////////////////////////////////////////////
// Texture/RenderTargetTextureResource_Creation.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Creation Functions of Render Target Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "RenderTargetTextureResource.h"

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Texture Creation Functions

		//------------------------------------------------------------------------------
		// Create a Render Target Texture Resource
		std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureResource(GraphicResourceManagerComponent *ownerManager,
			GLsizei width, GLsizei height, bool isEnableDepth, bool isEnableStencil,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, bool isUsePower2Dimension)
		{
			return CreateRenderTargetTextureResourceFromImageData(ownerManager, nullptr, 0, 0, width, height, isEnableDepth, isEnableStencil, minFilter, magFilter, wrapS, wrapT, isUsePower2Dimension);
		}

		//------------------------------------------------------------------------------
		// Create a Render Target Texture Resource with Given Image Data of unsigned char RGBA format
		std::shared_ptr<RenderTargetTextureResource> CreateRenderTargetTextureResourceFromImageData(GraphicResourceManagerComponent *ownerManager,
			const void *imageData, GLsizei imageWidth, GLsizei imageHeight, GLsizei textureWidth, GLsizei textureHeight, bool isEnableDepth, bool isEnableStencil,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, bool isUsePower2Dimension)
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

			GLint prevBinded;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBinded);

			GLint prevViewport[4];
			glGetIntegerv(GL_VIEWPORT, prevViewport);

			GLFrameBufferBindContext frameBufferRevertContext((GLuint)prevBinded, (GLint)prevViewport[0], (GLint)prevViewport[1], (GLsizei)prevViewport[2], (GLsizei)prevViewport[3], true);

			GLuint createdFrameBufferName = 0;
			glGenFramebuffers(1, &createdFrameBufferName);
			glBindFramebuffer(GL_FRAMEBUFFER, createdFrameBufferName);

			glGetError();

			GLuint createdDepthRenderBufferName = 0;
			if (isEnableDepth)
			{
				glGenRenderbuffers(1, &createdDepthRenderBufferName);
				glBindRenderbuffer(GL_RENDERBUFFER, createdDepthRenderBufferName);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, effectiveWidth, effectiveHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, createdDepthRenderBufferName);

				if (glGetError() != GL_NO_ERROR)
				{
					glDeleteFramebuffers(1, &createdFrameBufferName);
					return nullptr;
				}
			}

			GLuint createdStencilRenderBufferName = 0;
			if (isEnableStencil)
			{
				glGenRenderbuffers(1, &createdStencilRenderBufferName);
				glBindRenderbuffer(GL_RENDERBUFFER, createdStencilRenderBufferName);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, effectiveWidth, effectiveHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, createdStencilRenderBufferName);

				if (glGetError() != GL_NO_ERROR)
				{
					glDeleteFramebuffers(1, &createdFrameBufferName);
					if (isEnableDepth)
					{
						glDeleteRenderbuffers(1, &createdDepthRenderBufferName);
					}
					return nullptr;
				}
			}

			const void *creatingData = imageData;
			if (creatingData != nullptr)
			{
				if (imageWidth != textureWidth || imageHeight != textureHeight)
				{
					creatingData = nullptr;
				}
			}

			Texture::GLESRawTextureInformation creatingTextureInfo = Texture::Loader::LoadTextureFromBitmapDataInMemory(creatingData, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0, isUsePower2Dimension);
			if (!creatingTextureInfo.IsValid())
			{
				// Texture Creation Failed
				glDeleteFramebuffers(1, &createdFrameBufferName);
				if (isEnableDepth)
				{
					glDeleteRenderbuffers(1, &createdDepthRenderBufferName);
				}
				if (isEnableStencil)
				{
					glDeleteRenderbuffers(1, &createdStencilRenderBufferName);
				}
				return nullptr;
			}

			glBindTexture(GL_TEXTURE_2D, creatingTextureInfo.name);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
			if (creatingData == nullptr && imageData != nullptr)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
                #ifdef _LEGGIERO_IOS
                    glFlush();
                #else
                    glFinish();
                #endif
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, creatingTextureInfo.name, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				glDeleteFramebuffers(1, &createdFrameBufferName);
				if (isEnableDepth)
				{
					glDeleteRenderbuffers(1, &createdDepthRenderBufferName);
				}
				if (isEnableStencil)
				{
					glDeleteRenderbuffers(1, &createdStencilRenderBufferName);
				}
				glDeleteTextures(1, &(creatingTextureInfo.name));
				return nullptr;
			}

			return std::shared_ptr<RenderTargetTextureResource>(new RenderTargetTextureResource(ownerManager,
				creatingTextureInfo, createdFrameBufferName, createdDepthRenderBufferName, createdStencilRenderBufferName,
				textureWidth, textureHeight, isEnableDepth, isEnableStencil,
				minFilter, magFilter, wrapS, wrapT, isUsePower2Dimension));
		}
	}
}
