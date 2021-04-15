////////////////////////////////////////////////////////////////////////////////
// Toolbox/ModuledGame/ModuledGame.h (Leggiero - Engine)
//
// Provide Base Implemention for the Game as Composited Modules
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__TOOLBOX__MODULED_GAME__MODULED_GAME_H
#define __ENGINE__TOOLBOX__MODULED_GAME__MODULED_GAME_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <map>
#include <set>
#include <vector>

// Leggiero.Engine
#include "../../Module/EngineModuleId.h"
#include "../../Module/EngineComponentId.h"
#include "../../Application/BaseGame.h"


namespace Leggiero
{
	// Forward Declarations
	class EngineModuleInterface;
	class EngineComponent;


	namespace Toolbox
	{
		namespace Game
		{
			// Module Composited Game Base Class
			class ModuledGame
				: public Engine::BaseGame
			{
			public:
				ModuledGame();
				virtual ~ModuledGame();

			protected:	// IGameInitializer
				virtual void _InitializeGameEngineLibrary(IPlatformApplication *application) override;
				virtual void _FinalizeGameEngineLibrary() override;

				virtual void _AssembleGameEngineComponents(IPlatformApplication *application) override;
				virtual void _FinalizeGameEngineComponents() override;

			protected:
				void _InitializeModule(EngineModuleIdType moduleId, IPlatformApplication *application);
				void _RegisterEngineComponent(EngineComponent *component, bool isManagedByOther = false);

			protected:	// Composition interface by each game
				// List ALL modules
				static std::map<EngineModuleIdType, EngineModuleInterface *> _ListAllModules();

				virtual void _InitializeUsingModules(IPlatformApplication *application) = 0;
				virtual void _RegisterUsingComponents() = 0;

			private:
				std::map<EngineModuleIdType, EngineModuleInterface *>	m_moduleInterfaceTable;

				std::vector<EngineModuleIdType>		m_orderedModuleList;
				std::map<EngineModuleIdType, bool>	m_moduleInitilizationTable;

				std::vector<EngineComponentIdType>	m_orderedComponentList;
				std::set<EngineComponentIdType>		m_unManagedComponentSet;
			};
		}
	}
}

#endif
