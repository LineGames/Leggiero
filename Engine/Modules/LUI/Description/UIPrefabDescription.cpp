////////////////////////////////////////////////////////////////////////////////
// Description/UIPrefabDescription.cpp (Leggiero/Modules - LegacyUI)
//
// UI Prefab Description Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIPrefabDescription.h"

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Prefab/UIPrefabCommonObjects.h"
#include "../Prefab/UIFabricatedComponent.h"
#include "../Prefab/UIComponentPrefab.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// UIPrefabDescription

			//------------------------------------------------------------------------------
			void UIPrefabDescription::AddObjectToCreate(const std::shared_ptr<Prefab::IUIObjectPrefab> &objectPrefab)
			{
				if (!objectPrefab)
				{
					return;
				}
				m_objectsToCreate.push_back(objectPrefab);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIPrefabDescription::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				Prefab::ComponentPostProcessCreatingContext topLevelPostProcessContext;
				std::shared_ptr<UIObject> createdObject(_Fabricate(processingContext, creatingManager, topLevelPostProcessContext, placeProcessor));
				if (createdObject)
				{
					Prefab::ComponentPostProcessCreatingContext dummyPostProcessContext;
					topLevelPostProcessContext.DoPostProcessAfterFabrication(createdObject, processingContext, dummyPostProcessContext);
				}
				return createdObject;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIPrefabDescription::FabricateInParentContext(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, Prefab::ComponentPostProcessCreatingContext &parentPostProcessContext, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				return _Fabricate(processingContext, creatingManager, parentPostProcessContext, placeProcessor);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIPrefabDescription::_Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, Prefab::ComponentPostProcessCreatingContext &parentPostProcessContext, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				std::vector<std::shared_ptr<UIObject> > createdObjects;
				createdObjects.reserve(m_objectsToCreate.size());
				std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > idTable;

				for (std::shared_ptr<Prefab::IUIObjectPrefab> &objectPrefabToCreate : m_objectsToCreate)
				{
					std::shared_ptr<UIObject> fabricatedObject(objectPrefabToCreate->FabricateObject(processingContext, creatingManager, idTable, parentPostProcessContext, placeProcessor));
					if (fabricatedObject)
					{
						createdObjects.push_back(fabricatedObject);
					}
				}

				std::shared_ptr<UIObject> prefabFabricatedObject;
				if (createdObjects.size() == 1)
				{
					prefabFabricatedObject = createdObjects[0];
				}
				else
				{
					prefabFabricatedObject = std::make_shared<UIObject>(creatingManager);
					for (const std::shared_ptr<UIObject> &childObject : createdObjects)
					{
						prefabFabricatedObject->AddPostUIObject(childObject);
					}
				}

				prefabFabricatedObject->RegisterComponent(std::make_shared<UIFabricatedComponent>(idTable));
				return prefabFabricatedObject;
			}
		}
	}
}
