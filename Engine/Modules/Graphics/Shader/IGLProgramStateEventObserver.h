////////////////////////////////////////////////////////////////////////////////
// Shader/IGLProgramStateEventObserver.h (Leggiero/Modules - Graphics)
//
// Observer for OpenGL Shader Program State Events
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__I_GL_PROGRAM_STATE_EVENT_OBSERVER_H
#define __LM_GRAPHICS__SHADER__I_GL_PROGRAM_STATE_EVENT_OBSERVER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// External Library
#include <GLES3.h>


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declaration
		class GLShaderProgramResource;


		// Observer Interface to listen OpenGL Shader Program State Change Events
		class IGLProgramStateEventObserver
		{
		public:
			// Called when the Shader Program have been recompiled.
			// Occured after restoration due to context lost, and users must re-get location of uniforms, attributes, and so on.
			virtual void OnRecompiled(std::shared_ptr<GLShaderProgramResource> sender) { }

			// Called when the Shader Program is no more available.
			virtual void OnInvalidated(std::shared_ptr<GLShaderProgramResource> sender) { }
		};
	}
}

#endif
