////////////////////////////////////////////////////////////////////////////////
// _Internal/_InternalInitializer.h (Leggiero/Modules - Graphics)
//   * DO NOT directly include this header file outside of Graphics project
//
// Internal Initialization Function Definitions for Graphics Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS___INTERNAL___INTERNAL_INITIALIZER_H
#define __LM_GRAPHICS___INTERNAL___INTERNAL_INITIALIZER_H


namespace Leggiero
{
	// Forward Declarations
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Graphics
	{
		namespace _Internal
		{
			void _InitializeSharedInternalState();
			void _FinalizeSharedInternalState();

			void _InitializeReferenceState();
			void _InitializeReferenceStateInGraphicThread();
			void _InitializeApplicationReferenceState(Application::ApplicationComponent *appComponent);
			void _FinalizeReferenceState();
		}
	}
}

#endif
