////////////////////////////////////////////////////////////////////////////////
// Prefab/UIFabricatedComponent.cpp (Leggiero/Modules - LegacyUI)
//
// UI Fabricated Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIFabricatedComponent.h"

// Leggiero.LegacyUI
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIFabricatedComponent

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> UIFabricatedComponent::Clone(const UIObject &ownerObject) const
		{
			std::shared_ptr<UIFabricatedComponent> createdComponent(std::make_shared<UIFabricatedComponent>(m_idObjectTable));

			return createdComponent;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIObject> UIFabricatedComponent::FindObject(const UIFabricatedComponent::ObjectDescriptionIdType &findingId)
		{
			std::unordered_map<ObjectDescriptionIdType, std::weak_ptr<UIObject> >::iterator findIt = m_idObjectTable.find(findingId);
			if (findIt == m_idObjectTable.end())
			{
				return nullptr;
			}
			return findIt->second.lock();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIObject> UIFabricatedComponent::FindObjectByPath(const std::string &findingPathString)
		{
			Description::VariablePathType findingPath = Description::TypeUtility::ParseVariablePath(findingPathString);
			return _FindObjectByPath(findingPath, 0);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIObject> UIFabricatedComponent::_FindObjectByPath(const Description::VariablePathType &findingPath, size_t offset)
		{
			if (findingPath.size() <= offset)
			{
				return nullptr;
			}

			std::shared_ptr<UIObject> subObject(FindObject((ObjectDescriptionIdType)findingPath[offset]));
			if (!subObject)
			{
				return nullptr;
			}

			size_t nextOffset = offset + 1;
			if (nextOffset == findingPath.size())
			{
				return subObject;
			}
			else
			{
				std::shared_ptr<UIFabricatedComponent> subObjectFabricatedComponent(subObject->GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));
				if (subObjectFabricatedComponent)
				{
					return subObjectFabricatedComponent->_FindObjectByPath(findingPath, nextOffset);
				}
			}

			return nullptr;
		}
	}
}
