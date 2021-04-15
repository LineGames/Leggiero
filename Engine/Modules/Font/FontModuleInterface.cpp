////////////////////////////////////////////////////////////////////////////////
// FontModuleInterface.cpp (Leggiero/Modules - Font)
//
// Common Implementation related to Graphics Module
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FontModuleInterface.h"

// Leggiero.Engine
#include <Engine/Application/IPlatformApplication.h>
#include <Engine/Application/GameProcessAnchor.h>
#include <Engine/Application/GameProcessAnchorObserver.h>

// Leggiero.Font
#include "FreeType/FreeTypeFontCommon.h"


LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::Font::FontModule);
DEFINE_GET_SINGLETON_MODULE_INTERFACE(Leggiero::Font::FontModule, Leggiero::EngineModuleIdType::kFont);


namespace Leggiero
{
	namespace Font
	{
		//////////////////////////////////////////////////////////////////////////////// FontModule

		//------------------------------------------------------------------------------
		void FontModule::_InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor)
		{
			FreeType::InitializeFreeTypeLibrary();
		}

		//------------------------------------------------------------------------------
		void FontModule::_FinalizeCurrentModule()
		{
			FreeType::FinalizeFreeTypeLibrary();
		}
	}
}
