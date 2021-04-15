////////////////////////////////////////////////////////////////////////////////
// Shader/Basic/TexturedColorSimpleShader.h (Leggiero/Modules - Graphics)
//
// Textured Color Simple Shader Program
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__BASIC__TEXTURED_COLOR_SIMPLE_SHADER_H
#define __LM_GRAPHICS__SHADER__BASIC__TEXTURED_COLOR_SIMPLE_SHADER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include "../TexturedShaderProgram.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declaration
		class GraphicResourceManagerComponent;


		// Textured Color Simple Shader Program
		class TexturedColorSimpleShader
			: public TexturedShaderProgram
		{
		public:	// Creation Function
			static std::shared_ptr<TexturedColorSimpleShader> Create(GraphicResourceManagerComponent *resourceManager, bool isPremultipliedAlpha = false, bool isTintShader = false);

		public:
			TexturedColorSimpleShader(std::shared_ptr<GLShaderProgramResource> programResource);
			virtual ~TexturedColorSimpleShader();

		public:
			virtual GLint GetPositionSlot() const override { return m_positionSlot; }
			virtual GLint GetColorSlot() const override { return m_colorSlot; }
			virtual GLint GetTextureUVSlot() const override { return m_textureUVSlot; }

			virtual GLint GetModelMatrixLocation() const override { return m_modelMatrixLocation; }
			virtual GLint GetViewMatrixLocation() const override { return m_viewMatrixLocation; }
			virtual GLint GetProjectionMatrixLocation() const override { return m_projMatrixLocation; }
			virtual GLint GetTextureLocation() const override { return m_textureLocation; }

		public:
			virtual GLVertexAttribEnabledContext EnableUsingVertexAttribArray(bool isAutoDisable = false) override;
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

		public:	// Sources
			static const char *s_sourceVert;
			static const char *s_sourceFrag;
			static const char *s_sourceFrag_premulAlpha;
			static const char *s_sourceFrag_tint;
			static const char *s_sourceFrag_tint_premulAlpha;
		};
	}
}

#endif
