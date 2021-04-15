////////////////////////////////////////////////////////////////////////////////
// Rendering/UIShaders.h (Leggiero/Modules - LegacyUI)
//
// Shaders for UI
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_SHADERS_H
#define __LM_LUI__RENDERING__UI_SHADERS_H


// Leggiero.Graphics
#include <Graphics/Shader/CommonBaseShaderProgram.h>
#include <Graphics/Shader/TexturedShaderProgram.h>


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class GraphicResourceManagerComponent;
	}


	namespace LUI
	{
		namespace Shaders
		{
			// Color Primitive Shader Program
			class UIColorPrimitiveShader
				: public Graphics::CommonBaseShaderProgram
			{
			public:	// Creation Function
				static std::shared_ptr<UIColorPrimitiveShader> Create(Graphics::GraphicResourceManagerComponent *resourceManager);

			public:
				UIColorPrimitiveShader(std::shared_ptr<Graphics::GLShaderProgramResource> programResource);
				virtual ~UIColorPrimitiveShader();

			public:
				virtual GLint GetPositionSlot() const override { return m_positionSlot; }
				virtual GLint GetColorSlot() const override { return m_colorSlot; }

				virtual GLint GetModelMatrixLocation() const override { return m_modelMatrixLocation; }
				virtual GLint GetViewMatrixLocation() const override { return m_viewMatrixLocation; }
				virtual GLint GetProjectionMatrixLocation() const override { return m_projMatrixLocation; }

				GLint GetHSLModificationLocation() const { return m_hslModificationLocation; }

			public:
				virtual Graphics::GLVertexAttribEnabledContext EnableUsingVertexAttribArray(bool isAutoDisable = false) override;
				virtual void DisableAllUsingVertexAttribArray() override;

			protected:
				virtual void _RestoreAfterRecompile() override;

			protected:
				void _InitializeGLProgramLocations();

			protected:
				GLint m_positionSlot;
				GLint m_colorSlot;

				GLint m_modelMatrixLocation;
				GLint m_viewMatrixLocation;
				GLint m_projMatrixLocation;

				GLint m_hslModificationLocation;

			public:	// Sources
				static const char *s_sourceVert;
				static const char *s_sourceFrag;
			};


			// Textured Color Shader Program
			class UITextureColorShader
				: public Graphics::TexturedShaderProgram
			{
			public:	// Creation Function
				static std::shared_ptr<UITextureColorShader> Create(Graphics::GraphicResourceManagerComponent *resourceManager);

			public:
				UITextureColorShader(std::shared_ptr<Graphics::GLShaderProgramResource> programResource);
				virtual ~UITextureColorShader();

			public:
				virtual GLint GetPositionSlot() const override { return m_positionSlot; }
				virtual GLint GetColorSlot() const override { return m_colorSlot; }
				virtual GLint GetTextureUVSlot() const override { return m_textureUVSlot; }

				virtual GLint GetModelMatrixLocation() const override { return m_modelMatrixLocation; }
				virtual GLint GetViewMatrixLocation() const override { return m_viewMatrixLocation; }
				virtual GLint GetProjectionMatrixLocation() const override { return m_projMatrixLocation; }
				virtual GLint GetTextureLocation() const override { return m_textureLocation; }

				GLint GetHSLModificationLocation() const { return m_hslModificationLocation; }

			public:
				virtual Graphics::GLVertexAttribEnabledContext EnableUsingVertexAttribArray(bool isAutoDisable = false) override;
				virtual void DisableAllUsingVertexAttribArray() override;

			protected:
				virtual void _RestoreAfterRecompile() override;

			protected:
				void _InitializeGLProgramLocations();

			protected:
				GLint m_positionSlot;
				GLint m_colorSlot;
				GLint m_textureUVSlot;

				GLint m_modelMatrixLocation;
				GLint m_viewMatrixLocation;
				GLint m_projMatrixLocation;
				GLint m_textureLocation;

				GLint m_hslModificationLocation;

			public:	// Sources
				static const char *s_sourceVert;
				static const char *s_sourceFrag;
			};


			// Texture Gaussian Blur Shader Program
			class UITextureBlurShader
				: public Graphics::TexturedShaderProgram
			{
			public:	// Creation Function
				static std::shared_ptr<UITextureBlurShader> Create(Graphics::GraphicResourceManagerComponent *resourceManager);

			public:
				UITextureBlurShader(std::shared_ptr<Graphics::GLShaderProgramResource> programResource);
				virtual ~UITextureBlurShader();

			public:
				virtual GLint GetPositionSlot() const override { return m_positionSlot; }
				virtual GLint GetColorSlot() const override { return m_colorSlot; }
				virtual GLint GetTextureUVSlot() const override { return m_textureUVSlot; }

				virtual GLint GetModelMatrixLocation() const override { return m_modelMatrixLocation; }
				virtual GLint GetViewMatrixLocation() const override { return m_viewMatrixLocation; }
				virtual GLint GetProjectionMatrixLocation() const override { return m_projMatrixLocation; }
				virtual GLint GetTextureLocation() const override { return m_textureLocation; }
				GLint GetTexelWidthOffsetLocation() const { return m_texelWidthOffsetLocation; }
				GLint GetTexelHeightOffsetLocation() const { return m_texelHeightOffsetLocation; }

				GLint GetHSLModificationLocation() const { return m_hslModificationLocation; }

			public:
				virtual Graphics::GLVertexAttribEnabledContext EnableUsingVertexAttribArray(bool isAutoDisable = false) override;
				virtual void DisableAllUsingVertexAttribArray() override;

			protected:
				virtual void _RestoreAfterRecompile() override;

			protected:
				void _InitializeGLProgramLocations();

			protected:
				GLint m_positionSlot;
				GLint m_colorSlot;
				GLint m_textureUVSlot;

				GLint m_modelMatrixLocation;
				GLint m_viewMatrixLocation;
				GLint m_projMatrixLocation;
				GLint m_textureLocation;
				GLint m_texelWidthOffsetLocation;
				GLint m_texelHeightOffsetLocation;

				GLint m_hslModificationLocation;

			public:	// Sources
				static const char *s_sourceVert;
				static const char *s_sourceFrag;
			};
		}
	}
}

#endif
