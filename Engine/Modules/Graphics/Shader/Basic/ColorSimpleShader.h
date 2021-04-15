////////////////////////////////////////////////////////////////////////////////
// Shader/Basic/ColorSimpleShader.h (Leggiero/Modules - Graphics)
//
// Color Simple Shader Program
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__BASIC__COLOR_SIMPLE_SHADER_H
#define __LM_GRAPHICS__SHADER__BASIC__COLOR_SIMPLE_SHADER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include "../CommonBaseShaderProgram.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declaration
		class GraphicResourceManagerComponent;


		// Color Simple Shader Program
		class ColorSimpleShader
			: public CommonBaseShaderProgram
		{
		public:	// Creation Function
			static std::shared_ptr<ColorSimpleShader> Create(GraphicResourceManagerComponent *resourceManager, bool isPremultipliedAlpha = false);

		public:
			ColorSimpleShader(std::shared_ptr<GLShaderProgramResource> programResource);
			virtual ~ColorSimpleShader();

		public:
			virtual GLint GetPositionSlot() const override { return m_positionSlot; }
			virtual GLint GetColorSlot() const override { return m_colorSlot; }

			virtual GLint GetModelMatrixLocation() const override { return m_modelMatrixLocation; }
			virtual GLint GetViewMatrixLocation() const override { return m_viewMatrixLocation; }
			virtual GLint GetProjectionMatrixLocation() const override { return m_projMatrixLocation; }

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

			GLint m_modelMatrixLocation;
			GLint m_viewMatrixLocation;
			GLint m_projMatrixLocation;

		public:	// Sources
			static const char *s_sourceVert;
			static const char *s_sourceFrag;
			static const char *s_sourceFrag_premulAlpha;
		};
	}
}

#endif
