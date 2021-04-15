////////////////////////////////////////////////////////////////////////////////
// Common/GLFrameBufferBindContext.cpp (Leggiero/Modules - Graphics)
//
// Implementation of GLFrameBufferBindContext
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLFrameBufferBindContext.h"

// Leggiero.Graphics
#include "GraphicsReferenceState.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// GLFrameBufferBindContext

		//------------------------------------------------------------------------------
		GLFrameBufferBindContext::~GLFrameBufferBindContext()
		{
			if (m_isAutoUnBind)
			{
				_DoUnBind();
			}
		}

		//------------------------------------------------------------------------------
		GLFrameBufferBindContext::GLFrameBufferBindContext(GLFrameBufferBindContext &&other)
			: m_previousFrameBuffer(other.m_previousFrameBuffer)
			, m_previousViewportX(other.m_previousViewportX), m_previousViewportY(other.m_previousViewportY), m_previousViewportWidth(other.m_previousViewportWidth), m_previousViewportHeight(other.m_previousViewportHeight)
			, m_isUnBinded(other.m_isUnBinded), m_isAutoUnBind(other.m_isAutoUnBind)
		{
			other.m_isUnBinded = true;
		}

		//------------------------------------------------------------------------------
		GLFrameBufferBindContext &GLFrameBufferBindContext::operator=(GLFrameBufferBindContext &&other)
		{
			if (m_isAutoUnBind && !m_isUnBinded)
			{
				_DoUnBind();
			}

			m_previousFrameBuffer = other.m_previousFrameBuffer;

			m_previousViewportX = other.m_previousViewportX;
			m_previousViewportY = other.m_previousViewportY;
			m_previousViewportWidth = other.m_previousViewportWidth;
			m_previousViewportHeight = other.m_previousViewportHeight;

			m_isUnBinded = other.m_isUnBinded;
			m_isAutoUnBind = other.m_isAutoUnBind;

			other.m_isUnBinded = true;

			return *this;
		}

		//------------------------------------------------------------------------------
		// Un Bind Immediately
		void GLFrameBufferBindContext::UnBind()
		{
			_DoUnBind();
		}

		//------------------------------------------------------------------------------
		// Un Bind Processing FUnction
		void GLFrameBufferBindContext::_DoUnBind()
		{
			if (m_isUnBinded)
			{
				// Prevent Duplicated UnBind
				return;
			}

			m_isUnBinded = true;

			GLint currentBinded;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinded);

			GLint currentViewport[4];
			glGetIntegerv(GL_VIEWPORT, currentViewport);

			if (m_previousFrameBuffer != (GLuint)currentBinded)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_previousFrameBuffer);
			}
			if (m_previousViewportX != (GLint)currentViewport[0] || m_previousViewportY != (GLint)currentViewport[1]
				|| m_previousViewportWidth != (GLsizei)currentViewport[2] || m_previousViewportHeight != (GLsizei)currentViewport[3])
			{
				glViewport(m_previousViewportX, m_previousViewportY, m_previousViewportWidth, m_previousViewportHeight);
			}
		}

		//------------------------------------------------------------------------------
		// Bind default windows's frame buffer on the context of last binding
		GLFrameBufferBindContext GLFrameBufferBindContext::TemporarilyBindDefaultFrameBuffer(bool isAutoUnBind)
		{
			GLint prevBinded;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBinded);

			GLint prevViewport[4];
			glGetIntegerv(GL_VIEWPORT, prevViewport);

			GLuint defaultFrameBuffer = ReferenceState::GetReferenceFrameBufferObjectName();

			GLint defaultViewportX = ReferenceState::GetReferenceViewportX();
			GLint defaultViewportY = ReferenceState::GetReferenceViewportY();
			GLsizei defaultViewportWidth = ReferenceState::GetReferenceViewportWidth();
			GLsizei defaultViewportHeight = ReferenceState::GetReferenceViewportHeight();

			if (defaultFrameBuffer != (GLuint)prevBinded)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
			}
			if (defaultViewportX != (GLint)prevViewport[0] || defaultViewportY != (GLint)prevViewport[1]
				|| defaultViewportWidth != (GLsizei)prevViewport[2] || defaultViewportHeight != (GLsizei)prevViewport[3])
			{
				glViewport(defaultViewportX, defaultViewportY, defaultViewportWidth, defaultViewportHeight);
			}

			return GLFrameBufferBindContext((GLuint)prevBinded,
				(GLint)prevViewport[0], (GLint)prevViewport[1], (GLsizei)prevViewport[2], (GLsizei)prevViewport[3],
				isAutoUnBind);
		}
		//------------------------------------------------------------------------------
		// Bind given region of default windows's frame buffer on the context of last binding
		GLFrameBufferBindContext GLFrameBufferBindContext::TemporarilyBindDefaultFrameBufferInRect(GLint leftX, GLint topY, GLsizei width, GLsizei height, bool isAutoUnBind)
		{
			GLint prevBinded;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBinded);

			GLint prevViewport[4];
			glGetIntegerv(GL_VIEWPORT, prevViewport);

			GLuint defaultFrameBuffer = ReferenceState::GetReferenceFrameBufferObjectName();

			if (defaultFrameBuffer != (GLuint)prevBinded)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
			}
			if (leftX != (GLint)prevViewport[0] || topY != (GLint)prevViewport[1]
				|| width != (GLsizei)prevViewport[2] || height != (GLsizei)prevViewport[3])
			{
				glViewport(leftX, topY, width, height);
			}

			return GLFrameBufferBindContext((GLuint)prevBinded,
				(GLint)prevViewport[0], (GLint)prevViewport[1], (GLsizei)prevViewport[2], (GLsizei)prevViewport[3],
				isAutoUnBind);
		}
	}
}
