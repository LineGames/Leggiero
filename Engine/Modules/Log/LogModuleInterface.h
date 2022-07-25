////////////////////////////////////////////////////////////////////////////////
// LogModuleInterface.h (Leggiero/Modules - Log)
//
// Log Module Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__LOG_MODULE_ITNERFACE_H
#define __LM_LOG__LOG_MODULE_ITNERFACE_H


// Leggiero.Engine
#include <Engine/Module/EngineModuleInterface.h>


namespace Leggiero
{
	namespace Log
	{
		// Log Module Interface
		class LogModule
			: public EngineModuleInterface
			, public Utility::DesignPattern::Singleton<LogModule>
		{
			LEGGIERO_MAKE_SINGLETON_UNIQUE(LogModule);

		public:	// EngineModuleInterface
			// Get Type Id of the Module
			virtual EngineModuleIdType GetModuleType() const override { return EngineModuleIdType::kLog; };

		protected:	// EngineModuleInterface
			virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) override;
			virtual void _FinalizeCurrentModule() override;
		};
	}
}

LEGGIERO_DECLARE_SINGLETON_MEMBERS(Leggiero::Log::LogModule);


DECLARE_GET_MODULE_INTERFACE(Leggiero::Log::LogModule, Leggiero::EngineModuleIdType::kLog);

#endif
