////////////////////////////////////////////////////////////////////////////////
// Module/EngineComponent.h (Leggiero - Engine)
//
// Base class for an Engine Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_COMPONENT_H
#define __ENGINE__MODULE__ENGINE_COMPONENT_H


// Standard Library
#include <vector>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Engine
#include "EngineModuleId.h"
#include "EngineComponentId.h"


namespace Leggiero
{
	// Forward declarations
	class EngineComponentHolder;

	namespace Engine
	{
		class GameProcessAnchor;
	}


	// Base class for Leggiero Engine Component
	class EngineComponent
		: private Utility::SyntacticSugar::NonCopyable
	{
	public:
		EngineComponent() { }
		virtual ~EngineComponent() { }

	public:
		// Get Type Id of the Component
		virtual EngineComponentIdType GetComponentType() const { return EngineComponentIdType::kINVALID; };

		// Initialize the Component
		virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) { }

		// Safely Shutdown Component
		virtual void ShutdownComponent() { }

	public:
		// Get type Id list of dependent modules needed by this component
		virtual const std::vector<EngineModuleIdType> &GetDependentModules() const;

		// Get type Id list of dependent components needed by this component
		virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const;

		// Inject Dependency to the Component.
		// All dependency injections will be done before the initialization.
		virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) { }
	};
}

#endif
