////////////////////////////////////////////////////////////////////////////////
// HttpModuleInterface.h (Leggiero/Modules - HTTP)
//
// HTTP Module Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__HTTP_MODULE_ITNERFACE_H
#define __LM_HTTP__HTTP_MODULE_ITNERFACE_H


// Leggiero.Engine
#include <Engine/Module/EngineModuleInterface.h>


namespace Leggiero
{
	namespace HTTP
	{
		// HTTP Module Interface
		class HttpModule
			: public EngineModuleInterface
			, public Utility::DesignPattern::Singleton<HttpModule>
		{
			LEGGIERO_MAKE_SINGLETON_UNIQUE(HttpModule);

		public:	// EngineModuleInterface
			// Get Type Id of the Module
			virtual EngineModuleIdType GetModuleType() const override { return EngineModuleIdType::kHTTP; };

		protected:	// EngineModuleInterface
			virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) override;
			virtual void _FinalizeCurrentModule() override;
		};
	}
}

LEGGIERO_DECLARE_SINGLETON_MEMBERS(Leggiero::HTTP::HttpModule);


DECLARE_GET_MODULE_INTERFACE(Leggiero::HTTP::HttpModule, Leggiero::EngineModuleIdType::kHTTP);

#endif
