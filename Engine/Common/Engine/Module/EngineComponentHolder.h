////////////////////////////////////////////////////////////////////////////////
// Module/EngineComponentHolder.h (Leggiero - Engine)
//
// Base class for an Engine Component Holder (usually, the game engine)
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__MODULE__ENGINE_COMPONENT_HOLDER_H
#define __ENGINE__MODULE__ENGINE_COMPONENT_HOLDER_H


// Standard Library
#include <unordered_map>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.Engine
#include "EngineComponentId.h"


namespace Leggiero
{
	// Forward declaration for Component
	class EngineComponent;


	// Base class for Leggiero Engine Component Holder
	class EngineComponentHolder
	{
	public:
		EngineComponentHolder();
		virtual ~EngineComponentHolder() { }

	public:
		// Get engine component of given type
		template <class T>
		T *GetComponent() const { return nullptr; }

		// Get engine component of given type Id
		EngineComponent *GetComponentByTypeId(EngineComponentIdType componentType) const
		{
			auto findIt = m_components.find(componentType);
			if (findIt != m_components.cend())
			{
				return findIt->second;
			}
			return nullptr;
		}

	protected:
		void RegisterComponent(EngineComponent *component);
		void UnRegisterComponent(EngineComponentIdType componentType);

	private:
		std::unordered_map<EngineComponentIdType, EngineComponent *, Utility::SyntacticSugar::EnumClassHash> m_components;
	};
}


// Template specialization for Engine Component Getter Function
#define DECLARE_GET_COMPONENT_INTERFACE(CLASS_NAME, COMPONENT_ID) \
namespace Leggiero \
{ \
	template<> \
	CLASS_NAME *EngineComponentHolder::GetComponent<CLASS_NAME>() const; \
}

#define DEFINE_GET_COMPONENT_INTERFACE(CLASS_NAME, COMPONENT_ID) \
namespace Leggiero \
{ \
	template<> \
	CLASS_NAME *EngineComponentHolder::GetComponent<CLASS_NAME>() const \
	{ \
		return dynamic_cast<CLASS_NAME *>(GetComponentByTypeId(COMPONENT_ID)); \
	} \
}

#endif
