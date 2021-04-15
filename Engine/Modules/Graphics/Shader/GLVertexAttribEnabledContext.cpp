////////////////////////////////////////////////////////////////////////////////
// Shader/GLVertexAttribEnabledContext.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Vertex Attribute Enabled Context
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLVertexAttribEnabledContext.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// GLVertexAttribEnabledContext

		//------------------------------------------------------------------------------
		GLVertexAttribEnabledContext::~GLVertexAttribEnabledContext()
		{
			if (m_isAutoDisable)
			{
				_DoDisable();
			}
		}

		//------------------------------------------------------------------------------
		GLVertexAttribEnabledContext::GLVertexAttribEnabledContext(GLVertexAttribEnabledContext &&other)
			: m_enabledAttribArraySlots(other.m_enabledAttribArraySlots.begin(), other.m_enabledAttribArraySlots.end())
			, m_isDisabled(other.m_isDisabled), m_isAutoDisable(other.m_isAutoDisable)
		{
			other.m_enabledAttribArraySlots.clear();
		}

		//------------------------------------------------------------------------------
		GLVertexAttribEnabledContext &GLVertexAttribEnabledContext::operator=(GLVertexAttribEnabledContext &&other)
		{
			if (m_isAutoDisable
				&& (!m_isDisabled && m_enabledAttribArraySlots.size() > 0))
			{
				_DoDisable();
			}

			m_enabledAttribArraySlots.assign(other.m_enabledAttribArraySlots.begin(), other.m_enabledAttribArraySlots.end());

			m_isDisabled = other.m_isDisabled;
			m_isAutoDisable = other.m_isAutoDisable;

			other.m_enabledAttribArraySlots.clear();

			return *this;
		}

		//------------------------------------------------------------------------------
		// Do disable
		void GLVertexAttribEnabledContext::Disable()
		{
			_DoDisable();
		}

		//------------------------------------------------------------------------------
		// Disable process function
		void GLVertexAttribEnabledContext::_DoDisable()
		{
			if (m_isDisabled)
			{
				// Prevent Duplicated Disable
				return;
			}

			m_isDisabled = true;

			for (std::vector<GLint>::iterator slotIt = m_enabledAttribArraySlots.begin(); slotIt != m_enabledAttribArraySlots.end(); ++slotIt)
			{
				glDisableVertexAttribArray(*slotIt);
			}
			m_enabledAttribArraySlots.clear();
		}
	}
}
