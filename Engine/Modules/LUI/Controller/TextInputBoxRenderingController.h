////////////////////////////////////////////////////////////////////////////////
// Controller/TextInputBoxRenderingController.h (Leggiero/Modules - LegacyUI)
//
// Common rendering controller for text input box
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__CONTROLLER__TEXT_INPUT_BOX_RENDERING_CONTROLLER_H
#define __LM_LUI__CONTROLLER__TEXT_INPUT_BOX_RENDERING_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Element/UIElementTextInputBox.h"
#include "../Prefab/UIFabricatedComponent.h"
#include "CommonControllerDefines.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class UIObject;


		namespace Controller
		{
			// Simple Text Input Box Rendering Controller
			class SimpleFabricatedTextInputBoxRenderingController
				: public Element::UIElementTextInputBox::RenderingController
			{
			public:
				SimpleFabricatedTextInputBoxRenderingController(std::shared_ptr<UIObject> fabricatedBox,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingObjectId = CommonSubObjectNames::kNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingObjectId = CommonSubObjectNames::kPushBG,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingContentsObjectId = CommonSubObjectNames::kNormalContents, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingContentsObjectId = CommonSubObjectNames::kPushContents);

			public:
				virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::UIElementTextInputBox::StateType currentState, const GameDataString &currentTextValue) override;

			protected:
				std::shared_ptr<UIObject> normalBGRendering;
				std::shared_ptr<UIObject> pushedBGRendering;
				std::shared_ptr<UIObject> normalContentsRendering;
				std::shared_ptr<UIObject> pushedContentsRendering;
			};
		}
	}
}

#endif
