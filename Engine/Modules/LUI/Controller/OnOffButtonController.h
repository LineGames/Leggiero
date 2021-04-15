////////////////////////////////////////////////////////////////////////////////
// Controller/OnOffButtonController.h (Leggiero/Modules - LegacyUI)
//
// Basic On-Off button controller for custom object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__CONTROLLER__ON_OFF_BUTTON_CONTROLLER_H
#define __LM_LUI__CONTROLLER__ON_OFF_BUTTON_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Prefab/UIFabricatedComponent.h"
#include "CommonControllerDefines.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declaration
		class UIObject;
		class UITouch;


		namespace Controller
		{
			// Forward Declartion for mutual reference
			class OnOffButtonController;


			// On/Off button's each button controller
			class OnOffButtonEachButtonController
			{
			public:
				static const UIFabricatedComponent::ObjectDescriptionIdType kActiveContentsObjectName;
				static const UIFabricatedComponent::ObjectDescriptionIdType kInactiveContentsObjectName;
				static const UIFabricatedComponent::ObjectDescriptionIdType kSelectBGObjectName;

			public:
				OnOffButtonEachButtonController(std::shared_ptr<UIObject> buttonObject, OnOffButtonController *pParentController, bool isOnButton, UICoordinateRatioType touchHeightRatio);

			public:
				void SetActiveStateParameter(float activeParameter) { m_activeStateParameter = activeParameter; _UpdateUIForActiveState(); }
				float GetActiveStateParameter() const { return m_activeStateParameter; }

				std::shared_ptr<UIObject> GetSelectBGObject() { return m_selectBGObject; }

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);
				void _OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant);

				void _UpdateUIForActiveState();

			protected:
				std::shared_ptr<UIObject>	m_buttonObject;
				bool						m_isOnButton;

				float						m_activeStateParameter;

				std::shared_ptr<UIObject>	m_contentObjectActive;
				std::shared_ptr<UIObject>	m_contentObjectInactive;

				std::shared_ptr<UIObject>	m_selectBGObject;

				OnOffButtonController *m_pParentController;
			};


			// On/Off button controller
			class OnOffButtonController
			{
			public:
				static const UIFabricatedComponent::ObjectDescriptionIdType kOnButtonDefaultName;
				static const UIFabricatedComponent::ObjectDescriptionIdType kOffButtonDefaultName;

			public:
				using StateChangeHandlerFuncType = std::function<void(bool)>;

			public:
				OnOffButtonController(std::shared_ptr<UIObject> buttonSetObject, UICoordinateRatioType touchHeightRatio, UICoordinateType offButtonLeft, StateChangeHandlerFuncType onStateChanged, 
					const UIFabricatedComponent::ObjectDescriptionIdType &onButtonName = kOnButtonDefaultName, const UIFabricatedComponent::ObjectDescriptionIdType &offButtonName = kOffButtonDefaultName);
				virtual ~OnOffButtonController() { }

			public:
				void SetState(bool isOn, bool isSkipNotify = true);
				void SetOnStateChanged(StateChangeHandlerFuncType onStateChanged) { m_onStateChanged = onStateChanged; }

				void StopAnimationEffect();

			public:
				void UpdateFrame(GameTimeClockType::duration frameInterval);

			public:
				void NotifyChildTouchDown(std::shared_ptr<UITouch> inTouch, bool isOnButton);
				void NotifyChildTouchIn(std::shared_ptr<UITouch> inTouch, bool isOnButton);

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);

				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

			protected:
				virtual void _OnStateChanged(bool isOn) { }
				virtual void _OnTouchStateChanged(bool isDown) { }

			protected:
				std::shared_ptr<UIObject>	m_buttonSetObject;
				std::shared_ptr<UITouch>	m_processingTouch;

				bool						m_isOn;
				StateChangeHandlerFuncType	m_onStateChanged;

				std::shared_ptr<OnOffButtonEachButtonController> m_onButtonController;
				std::shared_ptr<OnOffButtonEachButtonController> m_offButtonController;

				bool m_isAnimationProcessing;
				float m_currentBlendParameter;
				const UICoordinateType m_offLeft;
				UICoordinateType m_animationStartOffset;
				float m_animationStartBlendParameter;
			};
		}
	}
}

#endif
