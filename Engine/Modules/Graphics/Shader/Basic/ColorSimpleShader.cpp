////////////////////////////////////////////////////////////////////////////////
// Shader/Basic/ColorSimpleShader.cpp (Leggiero/Modules - Graphics)
//
// Implementation of simple colored shader
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ColorSimpleShader.h"

// Leggiero.Graphics
#include "../../GraphicResourceManagerComponent.h"
#include "../ProgramManager.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// ColorSimpleShader

		//------------------------------------------------------------------------------
		ColorSimpleShader::ColorSimpleShader(std::shared_ptr<GLShaderProgramResource> programResource)
			: CommonBaseShaderProgram(programResource)
		{
			_InitializeGLProgramLocations();
		}

		//------------------------------------------------------------------------------
		ColorSimpleShader::~ColorSimpleShader()
		{
		}

		//------------------------------------------------------------------------------
		GLVertexAttribEnabledContext ColorSimpleShader::EnableUsingVertexAttribArray(bool isAutoDisable)
		{
			std::vector<GLint> enabledSlots;

			auto checkAndEnableFunc = [&enabledSlots](GLint currentSlot) mutable {
				GLint stateReadParameter;
				glGetVertexAttribiv(currentSlot, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &stateReadParameter);
				if (stateReadParameter == 0)
				{
					glEnableVertexAttribArray(currentSlot);
					enabledSlots.push_back(currentSlot);
				}
			};

			checkAndEnableFunc(GetPositionSlot());
			checkAndEnableFunc(GetColorSlot());

			return GLVertexAttribEnabledContext(enabledSlots.begin(), enabledSlots.end(), isAutoDisable);
		}

		//------------------------------------------------------------------------------
		void ColorSimpleShader::DisableAllUsingVertexAttribArray()
		{
			glDisableVertexAttribArray(GetPositionSlot());
			glDisableVertexAttribArray(GetColorSlot());
		}

		//------------------------------------------------------------------------------
		void ColorSimpleShader::_RestoreAfterRecompile()
		{
			_InitializeGLProgramLocations();
		}

		//------------------------------------------------------------------------------
		void ColorSimpleShader::_InitializeGLProgramLocations()
		{
			if (!m_programResource)
			{
				return;
			}

			// Attribute Setting
			m_positionSlot = m_programResource->GetAttribLocation("a_Position");
			m_colorSlot = m_programResource->GetAttribLocation("a_SourceColor");

			// Uniform Setting
			m_modelMatrixLocation = m_programResource->GetUniformLocation("u_ModelMatrix");
			m_viewMatrixLocation = m_programResource->GetUniformLocation("u_ViewMatrix");
			m_projMatrixLocation = m_programResource->GetUniformLocation("u_ProjMatrix");
		}


		//////////////////////////////////////////////////////////////////////////////// ColorSimpleShader - Creation

		//------------------------------------------------------------------------------
		std::shared_ptr<ColorSimpleShader> ColorSimpleShader::Create(GraphicResourceManagerComponent *resourceManager, bool isPremultipliedAlpha)
		{
			std::shared_ptr<GLShaderProgramResource> programResource = resourceManager->GetProgramManager().LoadGLProgramFromInMemoryCStrings(
				s_sourceVert, 
				(isPremultipliedAlpha ? s_sourceFrag_premulAlpha : s_sourceFrag), 
				0, 0, 
				true
			);
			if (!programResource)
			{
				// Program Resource Creation Failed
				return nullptr;
			}

			return std::make_shared<ColorSimpleShader>(programResource);
		}
	}
}
