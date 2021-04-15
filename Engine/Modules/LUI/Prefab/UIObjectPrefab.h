////////////////////////////////////////////////////////////////////////////////
// Prefab/UIObjectPrefab.h (Leggiero/Modules - LegacyUI)
//
// Prefab for UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_OBJECT_PREFAB_H
#define __LM_LUI__PREFAB__UI_OBJECT_PREFAB_H


// Standard Library
#include <memory>
#include <vector>

// Leggiero.LegacyUI
#include "../Description/UIDescriptionTypes.h"
#include "UIPrefabCommonObjects.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				// Forward Declarations
				class IUIComponentPrefab;
				struct ComponentPostProcessCreatingContext;


				// Prefab to fabricate UI Object
				class UIObjectPrefab
					: public IUIObjectPrefab
				{
				public:
					UIObjectPrefab() { }
					virtual ~UIObjectPrefab() { }

				public:	// IUIObjectPrefab
					virtual ObjectPrefabClassType GetClassType() const override { return ObjectPrefabClassType::kObject; }
					virtual std::shared_ptr<UIObject> FabricateObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &parentPostProcessContext, IUIPrefabPlaceProcessor *placeProcessor = nullptr) override;

				public:
					static std::shared_ptr<UIObjectPrefab> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:
					virtual void _ReadObjectCommonDataFromXMLElement(ParsingXMLElementType *elem);
					virtual bool _IsObjectSpecialXMLElement(ParsingXMLElementType *elem) { return false; }

					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor);

					virtual void _PostProcessAfterFabrication(std::shared_ptr<UIObject> createdObject, DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &postProcessContext, ComponentPostProcessCreatingContext &parentPostProcessContext);

				protected:
					std::vector<std::shared_ptr<IUIComponentPrefab> > m_componentsPrefab;

					std::vector<std::shared_ptr<IUIObjectPrefab> > m_preChildrenPrefab;
					std::vector<std::shared_ptr<IUIObjectPrefab> > m_postChildrenPrefab;
				};


				// Object placeholder for objects created by control code
				class ObjectPlaceHolder
					: public UIObjectPrefab
				{
				public:
					using CreateKeyType = std::string;

				public:
					virtual ~ObjectPlaceHolder() { }

				public:	// IUIObjectPrefab
					virtual ObjectPrefabClassType GetClassType() const override { return ObjectPrefabClassType::kPlaceholder; }

				public:
					void SetCreatingKey(CreateKeyType key) { m_creatingKey = key; }
					void AddParameter(std::string name, GameDataString data) { m_parameters[name] = data; }

				public:
					static std::shared_ptr<ObjectPlaceHolder> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					CreateKeyType									m_creatingKey;
					std::unordered_map<std::string, GameDataString>	m_parameters;
				};


				// Object created by importing another prefab
				class PrefabImportedObject
					: public UIObjectPrefab
				{
				public:
					PrefabImportedObject(VariablePathType &&prefabPath)
						: m_prefabPath(prefabPath)
					{ }

					PrefabImportedObject(const VariablePathType &prefabPath)
						: m_prefabPath(prefabPath)
					{ }

					virtual ~PrefabImportedObject() { }

				public:	// IUIObjectPrefab
					virtual ObjectPrefabClassType GetClassType() const override { return ObjectPrefabClassType::kPrefabImport; }

				public:
					static std::shared_ptr<PrefabImportedObject> ReadFromFromXMLElement(ParsingXMLElementType *elem);

				protected:	// UIObjectPrefab
					virtual std::shared_ptr<UIObject> _CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager,
						std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor) override;

				protected:
					VariablePathType m_prefabPath;
				};
			}
		}
	}
}

#endif
