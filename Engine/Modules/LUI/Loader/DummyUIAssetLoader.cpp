////////////////////////////////////////////////////////////////////////////////
// Loader/DummyUIAssetLoader.cpp (Leggiero/Modules - LegacyUI)
//
// Global Dummy Loader
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "DummyUIAssetLoader.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// DummyAssetLoader

		//------------------------------------------------------------------------------
		namespace _Internal
		{
			static DummyAssetLoader s_dummyLoader;
		}

		//------------------------------------------------------------------------------
		DummyAssetLoader &DummyAssetLoader::GetInstance()
		{
			return _Internal::s_dummyLoader;
		}
	}
}
