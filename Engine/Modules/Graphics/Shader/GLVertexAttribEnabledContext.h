////////////////////////////////////////////////////////////////////////////////
// Shader/GLVertexAttribEnabledContext.h (Leggiero/Modules - Graphics)
//
// OpenGL Vertex Attribute Enabled Result Context
// Use this to auto-disable
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__GL_VERTEX_ATTRIB_ENABLED_CONTEXT_H
#define __LM_GRAPHICS__SHADER__GL_VERTEX_ATTRIB_ENABLED_CONTEXT_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <vector>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>


namespace Leggiero
{
	namespace Graphics
	{
		// OpenGL Vertex Attribute Enabled Context
		class GLVertexAttribEnabledContext
			: private Utility::SyntacticSugar::NonCopyable
		{
		public:
			GLVertexAttribEnabledContext()
				: m_isDisabled(true)
			{ }

			template <class InputIterator>
			GLVertexAttribEnabledContext(InputIterator firstEnabledIter, InputIterator lastEnabledIter, bool isAutoDisable = false)
				: m_enabledAttribArraySlots(firstEnabledIter, lastEnabledIter)
				, m_isDisabled(false), m_isAutoDisable(isAutoDisable)
			{ }

			virtual ~GLVertexAttribEnabledContext();

		public:	// Can move context
			GLVertexAttribEnabledContext(GLVertexAttribEnabledContext &&other);
			GLVertexAttribEnabledContext &operator=(GLVertexAttribEnabledContext &&other);

		public:
			void Disable();

		protected:
			void _DoDisable();

		protected:
			std::vector<GLint> m_enabledAttribArraySlots;

			bool m_isDisabled;
			bool m_isAutoDisable;
		};
	}
}

#endif
