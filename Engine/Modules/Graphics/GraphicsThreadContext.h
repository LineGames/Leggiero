////////////////////////////////////////////////////////////////////////////////
// GraphicsThreadContext.h (Leggiero/Modules - Graphics)
//
// Graphics Context Related Stuffs for each Threads
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__GRAPHICS_THREAD_CONTEXT_H
#define __LM_GRAPHICS__GRAPHICS_THREAD_CONTEXT_H


// Standard Library
#include <memory>


namespace Leggiero
{
	namespace Graphics
	{
		// Common Base Interface for each Platform's Thread Context Informations
		class IGLThreadContextInformation
		{
		public:
			virtual ~IGLThreadContextInformation() { }
		};


		//////////////////////////////////////////////////////////////////////////////// Platform Dependent Functions
		// For following functions, implementation must be provided for each platform

		// Check whether if the initialization by GL context of the main rendering thread have done or not
		extern bool IsMainGLContextInitialized();

		// Initialize the main OpenGL Context
		extern void InitializeMainGLContextInGraphicThread();

		// Create called thread's OpenGL Context and return the infromation object
		extern std::shared_ptr<IGLThreadContextInformation> MakeThreadGLContext();

		// Clean up called thread's OpenGL Context
		extern void TerminateThreadGLContext(std::shared_ptr<IGLThreadContextInformation> contextInfo);

		// Check called thread's OpenGL Context's validity and restore it if needed
		extern void CheckAndRefreshThreadGLContext(std::shared_ptr<IGLThreadContextInformation> contextInfo);

		////////////////////////////////////////////////////////////////////////////////
	}
}

#endif
