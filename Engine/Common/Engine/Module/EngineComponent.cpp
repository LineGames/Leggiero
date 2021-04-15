////////////////////////////////////////////////////////////////////////////////
// Module/EngineComponent.cpp (Leggiero - Engine)
//
// Engine Component Base Implementation
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "EngineComponent.h"

// Leggiero.Engine
#include "EngineComponentHolder.h"


namespace Leggiero
{
	//////////////////////////////////////////////////////////////////////////////// EngineComponent

	//------------------------------------------------------------------------------
	// Get type Id list of dependent modules needed by this component
	const std::vector<EngineModuleIdType> &EngineComponent::GetDependentModules() const
	{
		static std::vector<EngineModuleIdType> dependentModuleList { };
		return dependentModuleList;
	}

	//------------------------------------------------------------------------------
	// Get type Id list of dependent components needed by this component
	const std::vector<EngineComponentIdType> &EngineComponent::GetDependentComponents() const
	{
		static std::vector<EngineComponentIdType> dependentComponentList { };
		return dependentComponentList;
	}
}
