////////////////////////////////////////////////////////////////////////////////
// Common/GLFrameBufferBindContext.h (Leggiero/Modules - Graphics)
//
// Automated Frame Buffer Bind Result Context Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__COMMON__GL_FRAME_BUFFER_BIND_CONTEXT_H
#define __LM_GRAPHICS__COMMON__GL_FRAME_BUFFER_BIND_CONTEXT_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>


namespace Leggiero
{
	namespace Graphics
	{
		// Frame Buffer Bind Result Context
		class GLFrameBufferBindContext
			: private Utility::SyntacticSugar::NonCopyable
		{
		public:
			GLFrameBufferBindContext()
				: m_isUnBinded(true)
			{ }

			GLFrameBufferBindContext(GLuint previousFrameBuffer, GLint previousViewportX, GLint previousViewportY, GLsizei previousViewportWidth, GLsizei previousViewportHeight, bool isAutoUnBind = false)
				: m_previousFrameBuffer(previousFrameBuffer)
				, m_previousViewportX(previousViewportX), m_previousViewportY(previousViewportY), m_previousViewportWidth(previousViewportWidth), m_previousViewportHeight(previousViewportHeight)
				, m_isUnBinded(false), m_isAutoUnBind(isAutoUnBind)
			{ }

			virtual ~GLFrameBufferBindContext();

		public:
			// Bind default windows's frame buffer on the context of last binding
			static GLFrameBufferBindContext TemporarilyBindDefaultFrameBuffer(bool isAutoUnBind = false);

			// Bind given region of default windows's frame buffer on the context of last binding
			static GLFrameBufferBindContext TemporarilyBindDefaultFrameBufferInRect(GLint leftX, GLint topY, GLsizei width, GLsizei height, bool isAutoUnBind = false);

		// Can move context
		public:
			GLFrameBufferBindContext(GLFrameBufferBindContext &&other);
			GLFrameBufferBindContext &operator=(GLFrameBufferBindContext &&other);

		public:
			// Un Bind Immediately
			void UnBind();

		protected:
			void _DoUnBind();

		protected:
			GLuint	m_previousFrameBuffer;
			GLint	m_previousViewportX;
			GLint	m_previousViewportY;
			GLsizei m_previousViewportWidth;
			GLsizei m_previousViewportHeight;

			bool m_isUnBinded;
			bool m_isAutoUnBind;
		};
	}
}

#endif
