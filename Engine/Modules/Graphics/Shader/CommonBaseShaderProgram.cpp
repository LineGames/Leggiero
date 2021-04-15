////////////////////////////////////////////////////////////////////////////////
// Shader/CommonBaseShaderProgram.cpp (Leggiero/Modules - Graphics)
//
// Implementation of common shader program base class
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CommonBaseShaderProgram.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// CommonBaseShaderProgram

		//------------------------------------------------------------------------------
		CommonBaseShaderProgram::CommonBaseShaderProgram(std::shared_ptr<GLShaderProgramResource> programResource)
			: m_programResource(programResource)
		{
			m_programResource->RegisterStateEventObserver(this);
		}

		//------------------------------------------------------------------------------
		CommonBaseShaderProgram::~CommonBaseShaderProgram()
		{
			m_programResource->UnRegisterStateEventObserver(this);
		}

		//------------------------------------------------------------------------------
		void CommonBaseShaderProgram::OnRecompiled(std::shared_ptr<GLShaderProgramResource> sender)
		{
			if (sender != m_programResource)
			{
				// cannot be...
				return;
			}

			_RestoreAfterRecompile();
		}
	}
}
