////////////////////////////////////////////////////////////////////////////////
// _Internal/_GraphicsModuleInternalState.h (Leggiero/Modules - Graphics)
//   * DO NOT directly include this header file outside of Graphics project
//
// Internal Shared States for Graphics Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS___INTERNAL___GRAPHICS_MODULE_INTERNAL_STATE_H
#define __LM_GRAPHICS___INTERNAL___GRAPHICS_MODULE_INTERNAL_STATE_H


namespace Leggiero
{
	// Forward Declarations
	namespace Utility
	{
		namespace Threading
		{
			class SafePthreadLock;
		}
	}


	namespace Graphics
	{
		namespace _Internal
		{
			namespace Synchronization
			{
				Utility::Threading::SafePthreadLock &GetTextureNameCreationLock();
			}
		}
	}
}

#endif
