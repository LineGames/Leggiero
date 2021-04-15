////////////////////////////////////////////////////////////////////////////////
// Texture/GLTextureResource.cpp (Leggiero/Modules - Graphics)
//
// Implementation of OpenGL ES Texture Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLTextureResource.h"

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>

// Leggiero.Graphics
#include "../GraphicResourceManagerComponent.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// GLTextureResource

		//------------------------------------------------------------------------------
		GLTextureResource::GLTextureResource(GraphicResourceManagerComponent *ownerManager,
			Texture::GLESRawTextureInformation textureInfo, std::function<Texture::GLESRawTextureInformation()> textureCreator,
			GLuint minFilter, GLuint magFilter, GLuint wrapS, GLuint wrapT)
			: m_isInvalidated(false), m_managerHolder(ownerManager->GetPointerHolder()), m_textureCreator(textureCreator)
			, m_minFilter(minFilter), m_magFilter(magFilter), m_wrapS(wrapS), m_wrapT(wrapT)
			, m_textureInfo(textureInfo)
		{
		}

		//------------------------------------------------------------------------------
		GLTextureResource::~GLTextureResource()
		{
			if (!m_isInvalidated && m_textureInfo.IsValid())
			{
				std::shared_ptr<Utility::Object::PointerHolder> ownerManagerHolder(m_managerHolder.lock());
				GraphicResourceManagerComponent *ownerManager = nullptr;
				std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
				if (ownerManagerHolder)
				{
					lockContext = ownerManagerHolder->LockTarget();
					if (lockContext)
					{
						ownerManager = lockContext->GetTypedObject<GraphicResourceManagerComponent>();
					}
				}
				if (ownerManager != nullptr)
				{
					ownerManager->TrashGLGarbageResources(_GenerateResourceTrashFunc());
				}
			}
		}

		//------------------------------------------------------------------------------
		// Bind the texture resource to given texture unit in OpenGL ES context
		bool GLTextureResource::Bind(GLenum textureUnit)
		{
			if (!m_textureInfo.IsValid())
			{
				return false;
			}

			glGetError();

			glActiveTexture(textureUnit);
			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);

			if (glGetError() != GL_NO_ERROR)
			{
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------------
		// Set texture's filtering scheme
		void GLTextureResource::SetTextureFilter(GLuint minFilter, GLuint magFilter)
		{
			if (!m_textureInfo.IsValid())
			{
				return;
			}
			if (minFilter == m_minFilter && magFilter == m_magFilter)
			{
				return;
			}

			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);
			if (minFilter != m_minFilter)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				m_minFilter = minFilter;
			}
			if (magFilter != m_magFilter)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
				m_magFilter = magFilter;
			}
		}

		//------------------------------------------------------------------------------
		// Set texture's wrap scheme
		void GLTextureResource::SetWarp(GLuint wrapS, GLuint wrapT)
		{
			if (!m_textureInfo.IsValid())
			{
				return;
			}
			if (wrapS == m_wrapS && wrapT == m_wrapT)
			{
				return;
			}

			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);
			if (wrapS != m_wrapS)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
				m_wrapS = wrapS;
			}
			if (wrapT != m_wrapT)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
				m_wrapT = wrapT;
			}
		}

		//------------------------------------------------------------------------------
		// Check validity of the texture resource.
		// Should be called in graphic thread with OpenGL context.
		bool GLTextureResource::IsValid()
		{
			if (m_isInvalidated)
			{
				return false;
			}

			if (m_textureInfo.IsValid() && glIsTexture(m_textureInfo.name) == GL_TRUE)
			{
				return true;
			}

			m_isInvalidated = true;
			return false;
		}

		//------------------------------------------------------------------------------
		// Restore invalidated texture resource.
		// Should be called in graphic thread with OpenGL context.
		bool GLTextureResource::Restore()
		{
			if (!m_isInvalidated)
			{
				if (IsValid())
				{
					// Not Need to Restore
					return true;
				}
			}

			m_textureInfo = m_textureCreator();
			if (!m_textureInfo.IsValid())
			{
				return false;
			}

			glBindTexture(GL_TEXTURE_2D, m_textureInfo.name);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);

			m_isInvalidated = false;

			return true;
		}

		//------------------------------------------------------------------------------
		// Create function object to finalize the resource
		std::function<void()> GLTextureResource::_GenerateResourceTrashFunc()
		{
			GLuint textureName = m_textureInfo.name;
			return [textureName]() mutable {
				if (glIsTexture(textureName) == GL_FALSE)
				{
					return;
				}
				glDeleteTextures(1, &textureName);
			};
		}
	}
}
