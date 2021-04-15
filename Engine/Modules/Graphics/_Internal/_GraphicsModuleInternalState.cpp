////////////////////////////////////////////////////////////////////////////////
// _Internal/_GraphicsModuleInternalState.cpp (Leggiero/Modules - Graphics)
//
// Implementation related to Graphics Module's Internal Shared States
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_GraphicsModuleInternalState.h"

// Leggiero.Utility
#include "Utility/Threading/ManagedThreadPrimitives.h"

// Leggiero.Graphics
#include "_InternalInitializer.h"


namespace Leggiero
{
	namespace Graphics
	{
		namespace _Internal
		{
			//////////////////////////////////////////////////////////////////////////////// Initializers

			//------------------------------------------------------------------------------
			void _InitializeSharedInternalState()
			{
			}

			//------------------------------------------------------------------------------
			void _FinalizeSharedInternalState()
			{
			}


			//////////////////////////////////////////////////////////////////////////////// Synchronization

			namespace Synchronization
			{
				//------------------------------------------------------------------------------
				Utility::Threading::SafePthreadLock g_textureNameCreationLock;

				//------------------------------------------------------------------------------
				Utility::Threading::SafePthreadLock &GetTextureNameCreationLock()
				{
					return g_textureNameCreationLock;
				}
			}
		}
	}
}
