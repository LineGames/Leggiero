////////////////////////////////////////////////////////////////////////////////
// FontModuleInterface.h (Leggiero/Modules - Font)
//
// Font Module Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FONT__FONT_MODULE_ITNERFACE_H
#define __LM_FONT__FONT_MODULE_ITNERFACE_H


// Leggiero.Engine
#include <Engine/Module/EngineModuleInterface.h>


namespace Leggiero
{
	namespace Font
	{
		// Graphics Module Interface
		class FontModule
			: public EngineModuleInterface
			, public Utility::DesignPattern::Singleton<FontModule>
		{
			LEGGIERO_MAKE_SINGLETON_UNIQUE(FontModule);

		public:	// EngineModuleInterface
			// Get Type Id of the Module
			virtual EngineModuleIdType GetModuleType() const override { return EngineModuleIdType::kFont; };

		protected:	// EngineModuleInterface
			virtual void _InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor) override;
			virtual void _FinalizeCurrentModule() override;
		};
	}
}

LEGGIERO_DECLARE_SINGLETON_MEMBERS(Leggiero::Font::FontModule);


DECLARE_GET_MODULE_INTERFACE(Leggiero::Font::FontModule, Leggiero::EngineModuleIdType::kFont);

#endif
