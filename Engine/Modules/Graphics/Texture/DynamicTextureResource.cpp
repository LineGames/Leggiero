////////////////////////////////////////////////////////////////////////////////
// Texture/DynamicTextureResource.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Dynamic Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "DynamicTextureResource.h"

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>

// Leggiero.Graphics
#include "../_Internal/_DummyInterfaces.h"
#include "TextureHelper.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Notifier

		namespace PredefinedDynamicTextureNotifier
		{
			const DynamicTextureRestoreNotifyDelegate kNonNotifyingDynamicTextureRestoreNotifier = DynamicTextureRestoreNotifyDelegate([](std::shared_ptr<DynamicTextureResource> _dummy) { });
		}


		//////////////////////////////////////////////////////////////////////////////// DynamicTextureResource

		//------------------------------------------------------------------------------
		DynamicTextureResource::DynamicTextureResource(GraphicResourceManagerComponent *ownerManager,
			Texture::GLESRawTextureInformation textureInfo,
			GLsizei width, GLsizei height,
			DynamicTextureRestoreNotifyDelegate restoreNotifier,
			GLenum format, GLenum dataType,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT, GLint mipmapLevel, bool isUsePower2Dimension)
			: GLTextureResource(ownerManager, textureInfo, std::bind(_Internal::_GLTextureResourceCreatorFunc_Dummy), minFilter, magFilter, wrapS, wrapT)
			, m_width(width), m_height(height), m_restoreNotifier(restoreNotifier)
			, m_format(format), m_dataType(dataType), m_mipmapLevel(mipmapLevel), m_isUsePower2Dimension(isUsePower2Dimension)
		{
		}

		//------------------------------------------------------------------------------
		DynamicTextureResource::~DynamicTextureResource()
		{
		}

		//------------------------------------------------------------------------------
		// Restore invalidated texture resource.
		// Should be called in graphic thread with OpenGL context.
		bool DynamicTextureResource::Restore()
		{
			if (!m_isInvalidated)
			{
				if (IsValid())
				{
					// Not Need to Restore
					return true;
				}
			}

			return RestoreWithImageData(nullptr, 0, 0, false);
		}

		//------------------------------------------------------------------------------
		// Restore invalidated texture resource without notification.
		// Should be called in graphic thread with OpenGL context.
		bool DynamicTextureResource::RestoreSlient()
		{
			if (!m_isInvalidated)
			{
				if (IsValid())
				{
					// Not Need to Restore
					return true;
				}
			}

			return RestoreWithImageData(nullptr, 0, 0, true);
		}

		//------------------------------------------------------------------------------
		// Restore invalidated texture resource with given image data (which shoud have same data format and type of the texture)
		// Should be called in graphic thread with OpenGL context.
		bool DynamicTextureResource::RestoreWithImageData(const void *imageData, GLsizei imageWidth, GLsizei imageHeight, bool isSuppressRestoreNotify)
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

			m_textureInfo = Texture::Loader::LoadTextureFromBitmapDataInMemory(creatingData, m_width, m_height, m_format, m_dataType, m_mipmapLevel, m_isUsePower2Dimension);
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
				glTexSubImage2D(GL_TEXTURE_2D, m_mipmapLevel, 0, 0, imageWidth, imageHeight, m_format, m_dataType, imageData);
                #ifdef _LEGGIERO_IOS
                    glFlush();
                #else
                    glFinish();
                #endif
			}

			m_isInvalidated = false;
			if (!isSuppressRestoreNotify)
			{
				m_restoreNotifier(shared_from_this());
			}
			return true;
		}

		//------------------------------------------------------------------------------
		// Upload image to texture's given sub-region. Image data's format and type should be matched with texture's.
		// Should be called in graphic thread with OpenGL context.
		void DynamicTextureResource::UploadSubImage(GLint x, GLint y, const void *imageData, GLsizei imageWidth, GLsizei imageHeight)
		{
			if (imageData == nullptr)
			{
				return;
			}
			if (imageWidth == 0 || imageHeight == 0)
			{
				return;
			}

			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);
			glTexSubImage2D(GL_TEXTURE_2D, m_mipmapLevel, x, y, imageWidth, imageHeight, m_format, m_dataType, imageData);
            #ifdef _LEGGIERO_IOS
                glFlush();
            #else
                glFinish();
            #endif
		}

		//------------------------------------------------------------------------------
		// Copy image from frame buffer's given sub-region
		// Should be called in graphic thread with OpenGL context.
		void DynamicTextureResource::CopyFromFrameBuffer(GLint sourceX, GLint sourceY, GLint x, GLint y, GLsizei imageWidth, GLsizei imageHeight)
		{
			if (imageWidth == 0 || imageHeight == 0)
			{
				return;
			}

			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);
			glCopyTexSubImage2D(GL_TEXTURE_2D, m_mipmapLevel, x, y, sourceX, sourceY, imageWidth, imageHeight);
		}
	}
}
