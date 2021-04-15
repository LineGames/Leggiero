////////////////////////////////////////////////////////////////////////////////
// Module/EngineComponentHolder.cpp (Leggiero - Engine)
//
// Engine Component Holder Implementation
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "EngineComponentHolder.h"

// Leggiero.Engine
#include "EngineComponent.h"


namespace Leggiero
{
	//////////////////////////////////////////////////////////////////////////////// EngineComponentHolder
	
	//------------------------------------------------------------------------------
	// Initialization
	EngineComponentHolder::EngineComponentHolder()
	{
	}

	//------------------------------------------------------------------------------
	// Register an Engine Component
	void EngineComponentHolder::RegisterComponent(EngineComponent *component)
	{
		if (component == nullptr)
		{
			// Null Component
			return;
		}

		EngineComponentIdType registerComponentType = component->GetComponentType();
		if (registerComponentType == EngineComponentIdType::kINVALID)
		{
			// Invalid Component
			return;
		}

		m_components[registerComponentType] = component;
	}

	//------------------------------------------------------------------------------
	// Remove an Engine Component
	void EngineComponentHolder::UnRegisterComponent(EngineComponentIdType componentType)
	{
		m_components.erase(componentType);
	}
}
