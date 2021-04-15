////////////////////////////////////////////////////////////////////////////////
// HttpModuleInterface.cpp (Leggiero/Modules - HTTP)
//
// Common Implementation related to HTTP Module
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HttpModuleInterface.h"

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.HTTP
#ifndef _LEGGIERO_IOS
#include "cURL/cURLUtility.h"
#endif


LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::HTTP::HttpModule);
DEFINE_GET_SINGLETON_MODULE_INTERFACE(Leggiero::HTTP::HttpModule, Leggiero::EngineModuleIdType::kHTTP);


namespace Leggiero
{
	namespace HTTP
	{
		//////////////////////////////////////////////////////////////////////////////// HttpModule

		//------------------------------------------------------------------------------
		void HttpModule::_InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor)
		{
			#ifndef _LEGGIERO_IOS
				cURL::InitializeModuleCUrlSystem();
			#endif
		}

		//------------------------------------------------------------------------------
		void HttpModule::_FinalizeCurrentModule()
		{
			#ifndef _LEGGIERO_IOS
				cURL::FinalizeModuleCUrlSystem();
			#endif
		}
	}
}
