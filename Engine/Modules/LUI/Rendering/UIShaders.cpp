////////////////////////////////////////////////////////////////////////////////
// Rendering/UIShaders.cpp (Leggiero/Modules - LegacyUI)
//
// UI Shaders Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIShaders.h"

// Standard Library
#include <vector>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>
#include <Graphics/Shader/ProgramManager.h>


namespace Leggiero
{
	namespace LUI
	{
		namespace Shaders
		{
			//////////////////////////////////////////////////////////////////////////////// UIColorPrimitiveShader

			//------------------------------------------------------------------------------
			UIColorPrimitiveShader::UIColorPrimitiveShader(std::shared_ptr<Graphics::GLShaderProgramResource> programResource)
				: CommonBaseShaderProgram(programResource)
			{
				_InitializeGLProgramLocations();
			}

			//------------------------------------------------------------------------------
			UIColorPrimitiveShader::~UIColorPrimitiveShader()
			{
			}

			//------------------------------------------------------------------------------
			Graphics::GLVertexAttribEnabledContext UIColorPrimitiveShader::EnableUsingVertexAttribArray(bool isAutoDisable)
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

				return Graphics::GLVertexAttribEnabledContext(enabledSlots.begin(), enabledSlots.end(), isAutoDisable);
			}

			//------------------------------------------------------------------------------
			void UIColorPrimitiveShader::DisableAllUsingVertexAttribArray()
			{
				glDisableVertexAttribArray(GetPositionSlot());
				glDisableVertexAttribArray(GetColorSlot());
			}

			//------------------------------------------------------------------------------
			void UIColorPrimitiveShader::_RestoreAfterRecompile()
			{
				_InitializeGLProgramLocations();
			}

			//------------------------------------------------------------------------------
			void UIColorPrimitiveShader::_InitializeGLProgramLocations()
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
				m_hslModificationLocation = m_programResource->GetUniformLocation("u_hslModify");
			}


			//////////////////////////////////////////////////////////////////////////////// UITextureColorShader

			//------------------------------------------------------------------------------
			UITextureColorShader::UITextureColorShader(std::shared_ptr<Graphics::GLShaderProgramResource> programResource)
				: TexturedShaderProgram(programResource)
			{
				_InitializeGLProgramLocations();
			}

			//------------------------------------------------------------------------------
			UITextureColorShader::~UITextureColorShader()
			{
			}

			//------------------------------------------------------------------------------
			Graphics::GLVertexAttribEnabledContext UITextureColorShader::EnableUsingVertexAttribArray(bool isAutoDisable)
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

				return Graphics::GLVertexAttribEnabledContext(enabledSlots.begin(), enabledSlots.end(), isAutoDisable);
			}

			//------------------------------------------------------------------------------
			void UITextureColorShader::DisableAllUsingVertexAttribArray()
			{
				glDisableVertexAttribArray(GetPositionSlot());
				glDisableVertexAttribArray(GetColorSlot());
				glDisableVertexAttribArray(GetTextureUVSlot());
			}

			//------------------------------------------------------------------------------
			void UITextureColorShader::_RestoreAfterRecompile()
			{
				_InitializeGLProgramLocations();
			}

			//------------------------------------------------------------------------------
			void UITextureColorShader::_InitializeGLProgramLocations()
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
				m_hslModificationLocation = m_programResource->GetUniformLocation("u_hslModify");
			}


			//////////////////////////////////////////////////////////////////////////////// UITextureBlurShader

			//------------------------------------------------------------------------------
			UITextureBlurShader::UITextureBlurShader(std::shared_ptr<Graphics::GLShaderProgramResource> programResource)
				: TexturedShaderProgram(programResource)
			{
				_InitializeGLProgramLocations();
			}

			//------------------------------------------------------------------------------
			UITextureBlurShader::~UITextureBlurShader()
			{
			}

			//------------------------------------------------------------------------------
			Graphics::GLVertexAttribEnabledContext UITextureBlurShader::EnableUsingVertexAttribArray(bool isAutoDisable)
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

				return Graphics::GLVertexAttribEnabledContext(enabledSlots.begin(), enabledSlots.end(), isAutoDisable);
			}

			//------------------------------------------------------------------------------
			void UITextureBlurShader::DisableAllUsingVertexAttribArray()
			{
				glDisableVertexAttribArray(GetPositionSlot());
				glDisableVertexAttribArray(GetColorSlot());
				glDisableVertexAttribArray(GetTextureUVSlot());
			}

			//------------------------------------------------------------------------------
			void UITextureBlurShader::_RestoreAfterRecompile()
			{
				_InitializeGLProgramLocations();
			}

			//------------------------------------------------------------------------------
			void UITextureBlurShader::_InitializeGLProgramLocations()
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
				m_texelWidthOffsetLocation = m_programResource->GetUniformLocation("u_texelWidthOffset");
				m_texelHeightOffsetLocation = m_programResource->GetUniformLocation("u_texelHeightOffset");
				m_hslModificationLocation = m_programResource->GetUniformLocation("u_hslModify");
			}


			//////////////////////////////////////////////////////////////////////////////// Creation Functions

			//------------------------------------------------------------------------------
			std::shared_ptr<UIColorPrimitiveShader> UIColorPrimitiveShader::Create(Graphics::GraphicResourceManagerComponent *resourceManager)
			{
				std::shared_ptr<Graphics::GLShaderProgramResource> programResource = resourceManager->GetProgramManager().LoadGLProgramFromInMemoryCStrings(
					s_sourceVert, s_sourceFrag,
					0, 0,
					true
				);
				if (!programResource)
				{
					// Program Resource Creation Failed
					return nullptr;
				}

				return std::make_shared<UIColorPrimitiveShader>(programResource);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UITextureColorShader> UITextureColorShader::Create(Graphics::GraphicResourceManagerComponent *resourceManager)
			{
				std::shared_ptr<Graphics::GLShaderProgramResource> programResource = resourceManager->GetProgramManager().LoadGLProgramFromInMemoryCStrings(
					s_sourceVert, s_sourceFrag,
					0, 0,
					true
				);
				if (!programResource)
				{
					// Program Resource Creation Failed
					return nullptr;
				}

				return std::make_shared<UITextureColorShader>(programResource);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UITextureBlurShader> UITextureBlurShader::Create(Graphics::GraphicResourceManagerComponent *resourceManager)
			{
				std::shared_ptr<Graphics::GLShaderProgramResource> programResource = resourceManager->GetProgramManager().LoadGLProgramFromInMemoryCStrings(
					s_sourceVert, s_sourceFrag,
					0, 0,
					true
				);
				if (!programResource)
				{
					// Program Resource Creation Failed
					return nullptr;
				}

				return std::make_shared<UITextureBlurShader>(programResource);
			}
		}
	}
}
