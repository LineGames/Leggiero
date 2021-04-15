////////////////////////////////////////////////////////////////////////////////
// Prefab/UIPrefabPlaceProcessor.h (Leggiero/Modules - LegacyUI)
//
// Processor interface for UI place holders
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__PREFAB__UI_PREFAB_PLACE_PROCESSOR_H
#define __LM_LUI__PREFAB__UI_PREFAB_PLACE_PROCESSOR_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <string>
#include <unordered_map>

// Leggiero.LegacyUI
#include "UIObjectPrefab.h"
#include "UITextObjectPrefab.h"
#include "UIComponentPrefab.h"
#include "../Description/UIDescriptionSourcedString.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;


		namespace Description
		{
			namespace Prefab
			{
				// Placeholder Processor
				class IUIPrefabPlaceProcessor
				{
				public:
					virtual ~IUIPrefabPlaceProcessor() { }

				public:
					virtual std::shared_ptr<UIObject> ProcessObjectPlaceholderFabrication(std::shared_ptr<UIManager> creatingManager, ObjectPlaceHolder::CreateKeyType creatingKey, std::unordered_map<std::string, GameDataString> &parameters, ComponentPostProcessCreatingContext &postProcessContext) { return nullptr; };
					virtual std::shared_ptr<IUIComponent> ProcessComponentPlaceholderFabrication(std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, ComponentPlaceHolder::CreateKeyType creatingKey, std::unordered_map<std::string, GameDataString> &parameters, ComponentPostProcessCreatingContext &postProcessingContext) { return nullptr; };

					virtual GameDataString ProcessStringGeneration(const SourcedStringEntry::StringGeneratingKeyType &generateKey) { return ""; };
				};
			}
		}
	}
}

#endif
