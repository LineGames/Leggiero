////////////////////////////////////////////////////////////////////////////////
// GraphicsModuleInterface.h (Leggiero/Modules - Graphics)
//
// Graphics Module Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__GRAPHICS_MODULE_ITNERFACE_H
#define __LM_GRAPHICS__GRAPHICS_MODULE_ITNERFACE_H


// Leggiero.Engine
#include <Engine/Module/EngineModuleInterface.h>


namespace Leggiero
{
	// Forward Declarations
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Graphics
	{
		// Graphics Module Interface
		class GraphicsModule
			: public EngineModuleInterface
			, public Utility::DesignPattern::Singleton<GraphicsModule>
		{
			LEGGIERO_MAKE_SINGLETON_UNIQUE(GraphicsModule);

		public:	// EngineModuleInterface
			// Get Type Id of the Module
			virtual EngineModuleIdType GetModuleType() const override { return EngineModuleIdType::kGraphics; };

		protected:	// EngineModuleInterface
			virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) override;
			virtual void _FinalizeCurrentModule() override;
		};
	}
}

LEGGIERO_DECLARE_SINGLETON_MEMBERS(Leggiero::Graphics::GraphicsModule);


DECLARE_GET_MODULE_INTERFACE(Leggiero::Graphics::GraphicsModule, Leggiero::EngineModuleIdType::kGraphics);

#endif
