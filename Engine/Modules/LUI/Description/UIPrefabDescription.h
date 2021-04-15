////////////////////////////////////////////////////////////////////////////////
// Description/UIPrefabDescription.h (Leggiero/Modules - LegacyUI)
//
// UI Description of a prefab that can be fabricated to an UI Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__DESCRIPTION__UI_PREFAB_DESCRIPTION_H
#define __LM_LUI__DESCRIPTION__UI_PREFAB_DESCRIPTION_H


// Standard Library
#include <memory>
#include <vector>

// Leggiero.LegacyUI
#include "UIDescriptionTypes.h"
#include "UIDescription.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;


		namespace Description
		{
			// Forward Declarations
			class DescriptionProcessingContext;

			namespace Prefab
			{
				class IUIObjectPrefab;
				class IUIPrefabPlaceProcessor;
				struct ComponentPostProcessCreatingContext;
			}


			// UI Prefab Description
			class UIPrefabDescription
				: public IDescriptionObject
			{
			public:
				UIPrefabDescription() { }
				virtual ~UIPrefabDescription() { }

			public:	// DescriptionObject
				virtual UIDescriptionObjectType GetType() const override { return UIDescriptionObjectType::kUIPrefab; }

			public:
				std::shared_ptr<UIObject> Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, Prefab::IUIPrefabPlaceProcessor *placeProcessor = nullptr);
				std::shared_ptr<UIObject> FabricateInParentContext(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, Prefab::ComponentPostProcessCreatingContext &parentPostProcessContext, Prefab::IUIPrefabPlaceProcessor *placeProcessor);

			public:
				void AddObjectToCreate(const std::shared_ptr<Prefab::IUIObjectPrefab> &objectPrefab);

			protected:
				std::shared_ptr<UIObject> _Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, Prefab::ComponentPostProcessCreatingContext &parentPostProcessContext, Prefab::IUIPrefabPlaceProcessor *placeProcessor);

			protected:
				std::vector<std::shared_ptr<Prefab::IUIObjectPrefab> > m_objectsToCreate;
			};
		}
	}
}

#endif
