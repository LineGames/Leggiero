////////////////////////////////////////////////////////////////////////////////
// Controller/OnOffButtonController.cpp (Leggiero/Modules - LegacyUI)
//
// On/Off button controller implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OnOffButtonController.h"

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Common/UICommonArea.h"
#include "../Touch/UITouch.h"
#include "../Touch/ValuedTouchComponent.h"
#include "../Component/UILayoutComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Controller
		{
			//////////////////////////////////////////////////////////////////////////////// OnOffButtonEachButtonController

			//------------------------------------------------------------------------------
			const UIFabricatedComponent::ObjectDescriptionIdType OnOffButtonEachButtonController::kActiveContentsObjectName("contentsSelect");
			const UIFabricatedComponent::ObjectDescriptionIdType OnOffButtonEachButtonController::kInactiveContentsObjectName("contentsNormal");
			const UIFabricatedComponent::ObjectDescriptionIdType OnOffButtonEachButtonController::kSelectBGObjectName("bgSelect");

			//------------------------------------------------------------------------------
			OnOffButtonEachButtonController::OnOffButtonEachButtonController(std::shared_ptr<UIObject> buttonObject, OnOffButtonController *pParentController, bool isOnButton, UICoordinateRatioType touchHeightRatio)
				: m_buttonObject(buttonObject), m_pParentController(pParentController), m_isOnButton(isOnButton)
				, m_activeStateParameter(0.0f)
			{
				std::shared_ptr<UIFabricatedComponent> buttonFabricatedComponent(buttonObject->GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));

				m_contentObjectActive = buttonFabricatedComponent->FindObject(kActiveContentsObjectName);
				m_contentObjectInactive = buttonFabricatedComponent->FindObject(kInactiveContentsObjectName);
				m_selectBGObject = buttonFabricatedComponent->FindObject(kSelectBGObjectName);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(buttonObject, kUICoordinateRatio_One, touchHeightRatio));
				touchComponent->SetTouchDownFunc(std::bind(&OnOffButtonEachButtonController::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&OnOffButtonEachButtonController::_OnTouchIn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				buttonObject->RegisterComponent(touchComponent);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonEachButtonController::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				m_pParentController->NotifyChildTouchDown(touch, m_isOnButton);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonEachButtonController::_OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant)
			{
				m_pParentController->NotifyChildTouchIn(touch, m_isOnButton);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonEachButtonController::_UpdateUIForActiveState()
			{
				if (m_activeStateParameter <= 0.0f)
				{
					m_contentObjectActive->SetIsVisible(false);
					m_contentObjectInactive->SetIsVisible(true);
					m_contentObjectInactive->SetAlpha(1.0f);
				}
				else if (m_activeStateParameter >= 1.0f)
				{
					m_contentObjectActive->SetIsVisible(true);
					m_contentObjectInactive->SetIsVisible(false);
					m_contentObjectActive->SetAlpha(1.0f);
				}
				else
				{
					m_contentObjectActive->SetIsVisible(true);
					m_contentObjectInactive->SetIsVisible(true);
					m_contentObjectActive->SetAlpha(m_activeStateParameter);
					m_contentObjectInactive->SetAlpha(1.0f - m_activeStateParameter);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// OnOffButtonController

			//------------------------------------------------------------------------------
			const UIFabricatedComponent::ObjectDescriptionIdType OnOffButtonController::kOnButtonDefaultName("btnOn");
			const UIFabricatedComponent::ObjectDescriptionIdType OnOffButtonController::kOffButtonDefaultName("btnOff");

			//------------------------------------------------------------------------------
			OnOffButtonController::OnOffButtonController(std::shared_ptr<UIObject> buttonSetObject, UICoordinateRatioType touchHeightRatio, UICoordinateType offButtonLeft, StateChangeHandlerFuncType onStateChanged, const UIFabricatedComponent::ObjectDescriptionIdType &onButtonName, const UIFabricatedComponent::ObjectDescriptionIdType &offButtonName)
				: m_buttonSetObject(buttonSetObject), m_onStateChanged(onStateChanged), m_offLeft(offButtonLeft)
				, m_isOn(false)
			{
				std::shared_ptr<UIFabricatedComponent> objectFabricatedComponent(buttonSetObject->GetComponentObject<UIFabricatedComponent>(UIComponentType::kFabricated));

				m_onButtonController = std::make_shared<OnOffButtonEachButtonController>(objectFabricatedComponent->FindObject(onButtonName), this, true, touchHeightRatio);
				m_offButtonController = std::make_shared<OnOffButtonEachButtonController>(objectFabricatedComponent->FindObject(offButtonName), this, false, touchHeightRatio);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(buttonSetObject, kUICoordinateRatio_One, touchHeightRatio));
				touchComponent->SetTouchDownFunc(std::bind(&OnOffButtonController::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchCancelFunc(std::bind(&OnOffButtonController::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&OnOffButtonController::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&OnOffButtonController::_OnPrimaryTouchLose, this, std::placeholders::_1));
				buttonSetObject->RegisterComponent(touchComponent);

				// Set Initial State as isOn == false
				StopAnimationEffect();
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::SetState(bool isOn, bool isSkipNotify)
			{
				if (isOn == m_isOn)
				{
					return;
				}
				m_isOn = isOn;
				_OnStateChanged(isOn);

				// Start Animation
				m_animationStartOffset = m_onButtonController->GetSelectBGObject()->GetLayoutComponent()->parentOffsetX;
				m_animationStartBlendParameter = m_currentBlendParameter;
				m_isAnimationProcessing = true;

				if (!isSkipNotify)
				{
					m_onStateChanged(m_isOn);
				}
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::StopAnimationEffect()
			{
				m_onButtonController->SetActiveStateParameter(m_isOn ? 1.0f : 0.0f);
				m_onButtonController->GetSelectBGObject()->SetIsVisible(m_isOn);
				m_onButtonController->GetSelectBGObject()->SetAlpha(m_isOn ? 1.0f : 0.0f);
				m_onButtonController->GetSelectBGObject()->GetLayoutComponent()->parentOffsetX = (m_isOn ? 0.0f : m_offLeft);
				m_offButtonController->SetActiveStateParameter(m_isOn ? 0.0f : 1.0f);
				m_offButtonController->GetSelectBGObject()->SetIsVisible(!m_isOn);
				m_offButtonController->GetSelectBGObject()->SetAlpha(m_isOn ? 0.0f : 1.0f);
				m_offButtonController->GetSelectBGObject()->GetLayoutComponent()->parentOffsetX = (m_isOn ? -m_offLeft : 0.0f);

				m_currentBlendParameter = m_isOn ? 1.0f : 0.0f;
				m_isAnimationProcessing = false;
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::UpdateFrame(GameTimeClockType::duration frameInterval)
			{
				if (!m_isAnimationProcessing)
				{
					return;
				}

				constexpr float animationDuration = 0.23f;
				constexpr float animationDurationInverse = 1.0f / animationDuration;

				float frameIntervalInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);

				if (m_isOn)
				{
					m_currentBlendParameter += frameIntervalInSec * animationDurationInverse;
					if (m_currentBlendParameter >= 1.0f)
					{
						StopAnimationEffect();
						return;
					}
				}
				else
				{
					m_currentBlendParameter -= frameIntervalInSec * animationDurationInverse;
					if (m_currentBlendParameter <= 0.0f)
					{
						StopAnimationEffect();
						return;
					}
				}

				float inverseBlendParameter = 1.0f - m_currentBlendParameter;

				UICoordinateType selectOffset = static_cast<UICoordinateType>(inverseBlendParameter * m_offLeft);
				if (m_isOn)
				{
					float offsetMoveParameter = (m_currentBlendParameter - m_animationStartBlendParameter) / (1.0f - m_animationStartBlendParameter);
					float easyedOffsetMoveParameterInverse = (1.0f - offsetMoveParameter) * (1.0f - offsetMoveParameter);
					selectOffset = m_animationStartOffset * easyedOffsetMoveParameterInverse;
				}
				else
				{
					float offsetMoveParameter = (m_animationStartBlendParameter - m_currentBlendParameter) / m_animationStartBlendParameter;
					float easyedOffsetMoveParameterInverse = (1.0f - offsetMoveParameter) * (1.0f - offsetMoveParameter);
					float easyedOffsetMoveParameter = 1.0f - easyedOffsetMoveParameterInverse;
					selectOffset = m_animationStartOffset * easyedOffsetMoveParameterInverse + easyedOffsetMoveParameter * m_offLeft;
				}

				m_onButtonController->SetActiveStateParameter(m_currentBlendParameter);
				m_onButtonController->GetSelectBGObject()->SetIsVisible(true);
				m_onButtonController->GetSelectBGObject()->SetAlpha(m_currentBlendParameter);
				m_onButtonController->GetSelectBGObject()->GetLayoutComponent()->parentOffsetX = selectOffset;
				m_offButtonController->SetActiveStateParameter(inverseBlendParameter);
				m_offButtonController->GetSelectBGObject()->SetIsVisible(true);
				m_offButtonController->GetSelectBGObject()->SetAlpha(inverseBlendParameter);
				m_offButtonController->GetSelectBGObject()->GetLayoutComponent()->parentOffsetX = selectOffset - m_offLeft;
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::NotifyChildTouchDown(std::shared_ptr<UITouch> inTouch, bool isOnButton)
			{
				SetState(isOnButton, false);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::NotifyChildTouchIn(std::shared_ptr<UITouch> inTouch, bool isOnButton)
			{
				if (!m_processingTouch || !inTouch)
				{
					return;
				}
				if (m_processingTouch != inTouch)
				{
					return;
				}

				SetState(isOnButton, false);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				if (m_processingTouch)
				{
					return;
				}

				touch->Subscribe(m_buttonSetObject, true);
				if (!touch->ClaimPrimary(m_buttonSetObject))
				{
					touch->UnSubscribe(m_buttonSetObject);
					return;
				}

				m_processingTouch = touch;
				_OnTouchStateChanged(true);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				m_processingTouch.reset();
				_OnTouchStateChanged(false);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				m_processingTouch.reset();
				_OnTouchStateChanged(false);
			}

			//------------------------------------------------------------------------------
			void OnOffButtonController::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
			{
				touch->UnSubscribe(m_buttonSetObject);
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				m_processingTouch.reset();
				_OnTouchStateChanged(false);
			}
		}
	}
}
