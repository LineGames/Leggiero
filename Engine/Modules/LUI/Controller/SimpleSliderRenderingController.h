////////////////////////////////////////////////////////////////////////////////
// Controller/SimpleSliderRenderingController.h (Leggiero/Modules - LegacyUI)
//
// Common rendering controller for simple slider
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__CONTROLLER__SIMPLE_SLIDER_RENDERING_CONTROLLER_H
#define __LM_LUI__CONTROLLER__SIMPLE_SLIDER_RENDERING_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Element/UIElementSimpleSlideBar.h"
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
			// Simple Scroll Bar based on Slider Rendering Controller
			class SimpleSliderBasicRenderingController
				: public Element::UIElementSimpleSlideBar::RenderingController
			{
			public:
				SimpleSliderBasicRenderingController(std::shared_ptr<UIObject> marker, std::shared_ptr<UIObject> bar, 
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingMarkerObjectId = CommonSubObjectNames::kNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingMarkerObjectId = CommonSubObjectNames::kPushBG,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingMarkerContentsObjectId = CommonSubObjectNames::kNormalContents, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingMarkerContentsObjectId = CommonSubObjectNames::kPushContents,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingBarObjectId = CommonSubObjectNames::kNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingBarObjectId = CommonSubObjectNames::kPushBG,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingBarContentsObjectId = CommonSubObjectNames::kNormalContents, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingBarContentsObjectId = CommonSubObjectNames::kPushContents);

			public:
				virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, bool isUsing) override;

			protected:
				std::shared_ptr<UIObject> normalBGRenderingBar;
				std::shared_ptr<UIObject> pushedBGRenderingBar;
				std::shared_ptr<UIObject> normalContentsRenderingBar;
				std::shared_ptr<UIObject> pushedContentsRenderingBar;

				std::shared_ptr<UIObject> normalBGRenderingMarker;
				std::shared_ptr<UIObject> pushedBGRenderingMarker;
				std::shared_ptr<UIObject> normalContentsRenderingMarker;
				std::shared_ptr<UIObject> pushedContentsRenderingMarker;
			};
		}
	}
}

#endif
