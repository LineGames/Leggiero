////////////////////////////////////////////////////////////////////////////////
// Controller/CommonButtonRenderingController.h (Leggiero/Modules - LegacyUI)
//
// Basic controller processing rendering functions for buttons
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__CONTROLLER__COMMON_BUTTON_RENDERING_CONTROLLER_H
#define __LM_LUI__CONTROLLER__COMMON_BUTTON_RENDERING_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Element/UIElementSingleSimpleButton.h"
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
			// Controller for simple pushable button
			class SimpleButtonRenderingController
				: public Element::SingleSimpleButton::RenderingController
			{
			public:
				SimpleButtonRenderingController(UIObject &button,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingObjectId = CommonSubObjectNames::kNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingObjectId = CommonSubObjectNames::kPushBG, 
					const UIFabricatedComponent::ObjectDescriptionIdType &normalRenderingContentsObjectId = CommonSubObjectNames::kNormalContents, const UIFabricatedComponent::ObjectDescriptionIdType &pushedRenderingContentsObjectId = CommonSubObjectNames::kPushContents);

			public:
				virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::SingleSimpleButton::StateType currentState) override;

			protected:
				std::shared_ptr<UIObject> normalRendering;
				std::shared_ptr<UIObject> pushedRendering;

				std::shared_ptr<UIObject> normalRenderingContents;
				std::shared_ptr<UIObject> pushedRenderingContents;
			};


			// Controller for buttons that can have activated state
			class ActivatableButtonRenderingController
				: public Element::SingleSimpleButton::RenderingController
			{
			public:
				ActivatableButtonRenderingController(UIObject &button,
					const UIFabricatedComponent::ObjectDescriptionIdType &normalBGId = CommonSubObjectNames::kNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &pushedBGId = CommonSubObjectNames::kPushBG, const UIFabricatedComponent::ObjectDescriptionIdType &activeBGId = CommonSubObjectNames::kActiveBG, 
					const UIFabricatedComponent::ObjectDescriptionIdType &normalContentsId = CommonSubObjectNames::kNormalContents, const UIFabricatedComponent::ObjectDescriptionIdType &pushedContentsId = CommonSubObjectNames::kPushContents, const UIFabricatedComponent::ObjectDescriptionIdType &inactiveContentsId = CommonSubObjectNames::kInactiveContents, const UIFabricatedComponent::ObjectDescriptionIdType &activeContentsId = CommonSubObjectNames::kActiveContents);

			public:
				virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::SingleSimpleButton::StateType currentState) override;

			protected:
				std::shared_ptr<UIObject>	m_bgNormal;
				std::shared_ptr<UIObject>	m_bgPush;
				std::shared_ptr<UIObject>	m_bgActive;

				std::shared_ptr<UIObject>	m_contentsNormal;
				std::shared_ptr<UIObject>	m_contentsPush;
				std::shared_ptr<UIObject>	m_contentsNonActive;
				std::shared_ptr<UIObject>	m_contentsActive;
			};


			// Rendering controller for buttons have activate/disabled state
			class ActiveDisableButtonRenderingController
				: public Element::SingleSimpleButton::RenderingController
			{
			public:
				ActiveDisableButtonRenderingController(UIObject &button,
					const UIFabricatedComponent::ObjectDescriptionIdType &inactiveCommonBGId = CommonSubObjectNames::kInactiveBG, const UIFabricatedComponent::ObjectDescriptionIdType &inactiveNormalBGId = CommonSubObjectNames::kNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &inactivePushedBGId = CommonSubObjectNames::kPushBG,
					const UIFabricatedComponent::ObjectDescriptionIdType &activeCommonBGId = CommonSubObjectNames::kActiveBG, const UIFabricatedComponent::ObjectDescriptionIdType &activeNormalBGId = CommonSubObjectNames::kActiveNormalBG, const UIFabricatedComponent::ObjectDescriptionIdType &activePushedBGId = CommonSubObjectNames::kActivePushBG,
					const UIFabricatedComponent::ObjectDescriptionIdType &disabledBGId = CommonSubObjectNames::kDisableBG,
					const UIFabricatedComponent::ObjectDescriptionIdType &inactiveCommonContentsId = CommonSubObjectNames::kInactiveContents, const UIFabricatedComponent::ObjectDescriptionIdType &inactiveNormalContentsId = CommonSubObjectNames::kNormalContents, const UIFabricatedComponent::ObjectDescriptionIdType &inactivePushedContentsId = CommonSubObjectNames::kPushContents,
					const UIFabricatedComponent::ObjectDescriptionIdType &activeCommonContentsId = CommonSubObjectNames::kActiveContents, const UIFabricatedComponent::ObjectDescriptionIdType &activeNormalContentsId = CommonSubObjectNames::kPushContents, const UIFabricatedComponent::ObjectDescriptionIdType &activePushedContentsId = CommonSubObjectNames::kActivePushContents,
					const UIFabricatedComponent::ObjectDescriptionIdType &disabledContentsId = CommonSubObjectNames::kDisableContents);

			public:
				virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, Element::SingleSimpleButton::StateType currentState) override;

			protected:
				std::shared_ptr<UIObject> inactiveCommonBG;
				std::shared_ptr<UIObject> normalBG;
				std::shared_ptr<UIObject> pushedBG;
				std::shared_ptr<UIObject> activeCommonBG;
				std::shared_ptr<UIObject> activeNormalBG;
				std::shared_ptr<UIObject> activePushedBG;
				std::shared_ptr<UIObject> disabledBG;

				std::shared_ptr<UIObject> inactiveCommonContents;
				std::shared_ptr<UIObject> normalContents;
				std::shared_ptr<UIObject> pushedContents;
				std::shared_ptr<UIObject> activeCommonContents;
				std::shared_ptr<UIObject> activeNormalContents;
				std::shared_ptr<UIObject> activePushedContents;
				std::shared_ptr<UIObject> disabledContents;
			};
		}
	}
}

#endif
