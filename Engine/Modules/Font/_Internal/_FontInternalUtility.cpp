////////////////////////////////////////////////////////////////////////////////
// _Internal/_FontInternalUtility.cpp (Leggiero/Modules - Font)
//
// Internal Utility Implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_FontInternalUtility.h"

// Standard Library
#include <atomic>


namespace Leggiero
{
	namespace Font
	{
		namespace _Internal
		{
			//------------------------------------------------------------------------------
			FontIdType IssueFontId()
			{
				static std::atomic<FontIdType> g_issuedFontId(1);
				return g_issuedFontId.fetch_add(1);
			}
		}
	}
}
