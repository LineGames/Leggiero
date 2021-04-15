////////////////////////////////////////////////////////////////////////////////
// LogModuleInterface.cpp (Leggiero/Modules - Log)
//
// Common Implementation related to Log Module
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "LogModuleInterface.h"

// Leggiero.Log
#include "_Internal/_DebugLoggerInterface.h"


LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::Log::LogModule);
DEFINE_GET_SINGLETON_MODULE_INTERFACE(Leggiero::Log::LogModule, Leggiero::EngineModuleIdType::kLog);


namespace Leggiero
{
	namespace Log
	{
		//////////////////////////////////////////////////////////////////////////////// LogModule

		//------------------------------------------------------------------------------
		void LogModule::_InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor)
		{
			_Internal::_InitializeDebugLogger();
		}

		//------------------------------------------------------------------------------
		void LogModule::_FinalizeCurrentModule()
		{
			_Internal::_FinalizeDebugLogger();
		}
	}
}
