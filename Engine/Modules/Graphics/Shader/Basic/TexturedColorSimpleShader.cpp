////////////////////////////////////////////////////////////////////////////////
// Shader/Basic/TexturedColorSimpleShader.cpp (Leggiero/Modules - Graphics)
//
// Implementation of simple textured/colored shader
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TexturedColorSimpleShader.h"

// Leggiero.Graphics
#include "../../GraphicResourceManagerComponent.h"
#include "../ProgramManager.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// TexturedColorSimpleShader

		//------------------------------------------------------------------------------
		TexturedColorSimpleShader::TexturedColorSimpleShader(std::shared_ptr<GLShaderProgramResource> programResource)
			: TexturedShaderProgram(programResource)
		{
			_InitializeGLProgramLocations();
		}

		//------------------------------------------------------------------------------
		TexturedColorSimpleShader::~TexturedColorSimpleShader()
		{
		}

		//------------------------------------------------------------------------------
		GLVertexAttribEnabledContext TexturedColorSimpleShader::EnableUsingVertexAttribArray(bool isAutoDisable)
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
			checkAndEnableFunc(GetTextureUVSlot());

			return GLVertexAttribEnabledContext(enabledSlots.begin(), enabledSlots.end(), isAutoDisable);
		}

		//------------------------------------------------------------------------------
		void TexturedColorSimpleShader::DisableAllUsingVertexAttribArray()
		{
			glDisableVertexAttribArray(GetPositionSlot());
			glDisableVertexAttribArray(GetColorSlot());
			glDisableVertexAttribArray(GetTextureUVSlot());
		}

		//------------------------------------------------------------------------------
		void TexturedColorSimpleShader::_RestoreAfterRecompile()
		{
			_InitializeGLProgramLocations();
		}

		//------------------------------------------------------------------------------
		void TexturedColorSimpleShader::_InitializeGLProgramLocations()
		{
			if (!m_programResource)
			{
				return;
			}

			// Attribute Setting
			m_positionSlot = m_programResource->GetAttribLocation("a_Position");
			m_colorSlot = m_programResource->GetAttribLocation("a_SourceColor");
			m_textureUVSlot = m_programResource->GetAttribLocation("a_TextureUV0");

			// Uniform Setting
			m_modelMatrixLocation = m_programResource->GetUniformLocation("u_ModelMatrix");
			m_viewMatrixLocation = m_programResource->GetUniformLocation("u_ViewMatrix");
			m_projMatrixLocation = m_programResource->GetUniformLocation("u_ProjMatrix");
			m_textureLocation = m_programResource->GetUniformLocation("u_Texture0");
		}


		//////////////////////////////////////////////////////////////////////////////// ColorSimpleShader - Creation

		//------------------------------------------------------------------------------
		std::shared_ptr<TexturedColorSimpleShader> TexturedColorSimpleShader::Create(GraphicResourceManagerComponent *resourceManager, bool isPremultipliedAlpha, bool isTintShader)
		{
			const char *fragmentShader = s_sourceFrag;
			if (isTintShader)
			{
				if (isPremultipliedAlpha)
				{
					fragmentShader = s_sourceFrag_tint_premulAlpha;
				}
				else
				{
					fragmentShader = s_sourceFrag_tint;
				}
			}
			else if (isPremultipliedAlpha)
			{
				fragmentShader = s_sourceFrag_premulAlpha;
			}

			std::shared_ptr<GLShaderProgramResource> programResource = resourceManager->GetProgramManager().LoadGLProgramFromInMemoryCStrings(
				s_sourceVert, fragmentShader,
				0, 0, 
				true
			);
			if (!programResource)
			{
				// Program Resource Creation Failed
				return nullptr;
			}

			return std::make_shared<TexturedColorSimpleShader>(programResource);
		}
	}
}
