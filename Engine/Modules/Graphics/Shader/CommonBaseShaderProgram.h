////////////////////////////////////////////////////////////////////////////////
// Shader/CommonBaseShaderProgram.h (Leggiero/Modules - Graphics)
//
// Base class for common shader program object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__COMMON_BASE_SHADER_PROGRAM_H
#define __LM_GRAPHICS__SHADER__COMMON_BASE_SHADER_PROGRAM_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include "GLVertexAttribEnabledContext.h"
#include "IGLProgramStateEventObserver.h"
#include "GLShaderProgramResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		// Shader Program Base Class
		class CommonBaseShaderProgram
			: public IGLProgramStateEventObserver
			, private Utility::SyntacticSugar::NonCopyable
		{
		public:
			CommonBaseShaderProgram(std::shared_ptr<GLShaderProgramResource> programResource);
			virtual ~CommonBaseShaderProgram();

		public:	// IGLProgramStateEventObserver
			virtual void OnRecompiled(std::shared_ptr<GLShaderProgramResource> sender) override;

		public:	// GLProgramResource Wrapper
			bool Use() { return m_programResource->Use(); }
			GLint GetAttribLocation(const GLchar *name) { return m_programResource->GetAttribLocation(name); }
			GLint GetUniformLocation(const GLchar *name) { return m_programResource->GetUniformLocation(name); }

		public:
			virtual GLint GetPositionSlot() const = 0;
			virtual GLint GetColorSlot() const = 0;

			virtual GLint GetModelMatrixLocation() const = 0;
			virtual GLint GetViewMatrixLocation() const = 0;
			virtual GLint GetProjectionMatrixLocation() const = 0;

		public:
			virtual GLVertexAttribEnabledContext EnableUsingVertexAttribArray(bool isAutoDisable = false) { return GLVertexAttribEnabledContext(); }
			virtual void DisableAllUsingVertexAttribArray() { }

		protected:
			virtual void _RestoreAfterRecompile() { }

		protected:
			std::shared_ptr<GLShaderProgramResource> m_programResource;
		};
	}
}

#endif
