////////////////////////////////////////////////////////////////////////////////
// FreeType/FreeTypeFontCommon.cpp (Leggiero/Modules - Font)
//
// Common Function Implementation for FreeType Based Font
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FreeTypeFontCommon.h"


namespace Leggiero
{
	namespace Font
	{
		namespace FreeType
		{
			namespace _Internal
			{
				FT_Library							g_libraryInstance;
				Utility::Threading::SafePthreadLock	g_libraryFaceGenerateLock;
			}

			//------------------------------------------------------------------------------
			void InitializeFreeTypeLibrary()
			{
				FT_Error initError = FT_Init_FreeType(&_Internal::g_libraryInstance);
				if (initError != FT_Err_Ok)
				{
					// FreeType Library Initialization FAILED
				}
			}

			//------------------------------------------------------------------------------
			void FinalizeFreeTypeLibrary()
			{
			}
		}
	}
}
