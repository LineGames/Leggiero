////////////////////////////////////////////////////////////////////////////////
// Element/UIElementCancelPanelArea.cpp (Leggiero/Modules - LegacyUI)
//
// Cancel Panel Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementCancelPanelArea.h"

// Leggiero.LegacyUI
#include "../Common/UICommonArea.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Rendering/ShapeSetRenderingComponent.h"
#include "../Rendering/UIShapeRect.h"
#include "../Touch/UITouch.h"
#include "../Touch/ValuedTouchComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// CancelPanelArea

			// Dummy Handler Function
			CancelPanelArea::OnCancelHandlerType CancelPanelArea::ms_dummyOnCancelHandler([](std::shared_ptr<CancelPanelArea> _1) { });

			//------------------------------------------------------------------------------
			CancelPanelArea::CancelPanelArea(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager, nullptr, true)
				, m_onCancelHandlerFunc(ms_dummyOnCancelHandler)
				, m_isPressed(false), m_isLocked(false), m_moveThreshold(static_cast<UICoordinateType>(0.0f))
			{
			}

			//------------------------------------------------------------------------------
			CancelPanelArea::CancelPanelArea(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<IUIArea> cancelTouchArea, UICoordinateType moveThresholdAmount,
				std::shared_ptr<IUIArea> coveringArea, bool isInitialLocked, bool isLazyLayout, OnCancelHandlerType onCancel)
				: UIObject(ownerManager, isLazyLayout ? nullptr : std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))), isLazyLayout)
				, m_onCancelHandlerFunc(onCancel)
				, m_isPressed(false), m_isLocked(false), m_moveThreshold(moveThresholdAmount)
			{
				// Touch Handler
				m_touchComponent = std::make_shared<ValuedTouchComponent>();
				m_touchComponent->SetInteractionArea(cancelTouchArea);
				if (coveringArea)
				{
					m_touchComponent->SetCoverArea(coveringArea);
				}
				m_touchComponent->SetTouchDownFunc(std::bind(&CancelPanelArea::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				m_touchComponent->SetTouchMoveFunc(std::bind(&CancelPanelArea::_OnTouchMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetTouchOutFunc(std::bind(&CancelPanelArea::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetTouchCoveredFunc(std::bind(&CancelPanelArea::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetTouchCancelFunc(std::bind(&CancelPanelArea::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				m_touchComponent->SetTouchUpFunc(std::bind(&CancelPanelArea::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetPrimaryTouchLoseFunc(std::bind(&CancelPanelArea::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(m_touchComponent);
			}

			//------------------------------------------------------------------------------
			CancelPanelArea::CancelPanelArea(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<IUIArea> cancelTouchArea, UICoordinateType moveThresholdAmount,
				std::shared_ptr<IUIArea> coveringArea, bool isInitialLocked, std::shared_ptr<UILayoutComponent> layout, OnCancelHandlerType onCancel)
				: UIObject(ownerManager, layout, (layout == nullptr))
				, m_onCancelHandlerFunc(onCancel)
				, m_isPressed(false), m_isLocked(false), m_moveThreshold(moveThresholdAmount)
			{
				// Touch Handler
				m_touchComponent = std::make_shared<ValuedTouchComponent>();
				m_touchComponent->SetInteractionArea(cancelTouchArea);
				if (coveringArea)
				{
					m_touchComponent->SetCoverArea(coveringArea);
				}
				m_touchComponent->SetTouchDownFunc(std::bind(&CancelPanelArea::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				m_touchComponent->SetTouchMoveFunc(std::bind(&CancelPanelArea::_OnTouchMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetTouchOutFunc(std::bind(&CancelPanelArea::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetTouchCoveredFunc(std::bind(&CancelPanelArea::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetTouchCancelFunc(std::bind(&CancelPanelArea::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				m_touchComponent->SetTouchUpFunc(std::bind(&CancelPanelArea::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				m_touchComponent->SetPrimaryTouchLoseFunc(std::bind(&CancelPanelArea::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(m_touchComponent);
			}

			//------------------------------------------------------------------------------
			CancelPanelArea::~CancelPanelArea()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> CancelPanelArea::Clone() const
			{
				std::shared_ptr<UIObject> clonedObject(UIObject::Clone());
				std::shared_ptr<CancelPanelArea> concreteCloned(std::dynamic_pointer_cast<CancelPanelArea>(clonedObject));
				if (!concreteCloned)
				{
					return clonedObject;
				}

				concreteCloned->m_moveThreshold = m_moveThreshold;
				concreteCloned->m_onCancelHandlerFunc = m_onCancelHandlerFunc;

				concreteCloned->m_touchComponent = concreteCloned->GetComponentObject<ValuedTouchComponent>(UIComponentType::kTouchInteraction);

				return concreteCloned;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> CancelPanelArea::_AllocateCloningObject() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}
				return std::shared_ptr<UIObject>(dynamic_cast<UIObject *>(new CancelPanelArea(managerCopy)));
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::SetCancelArea(std::shared_ptr<IUIArea> area)
			{
				if (area)
				{
					m_touchComponent->SetInteractionArea(area);
				}
				else
				{
					m_touchComponent->SetInteractionArea(IUIArea::kNoArea);
				}
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::SetCoveringArea(std::shared_ptr<IUIArea> area)
			{
				if (area)
				{
					m_touchComponent->SetCoverArea(area);
				}
				else
				{
					m_touchComponent->SetCoverArea(IUIArea::kNoArea);
				}
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::CancelCurrentTouch()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
					m_processingTouch.reset();
				}
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				if (m_isLocked)
				{
					return;
				}
				if (m_processingTouch)
				{
					return;
				}
				if (touch->IsPrimaryOccupied())
				{
					return;
				}
				std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
				touch->Subscribe(sharedThisLValue, true);
				if (!touch->ClaimPrimary(sharedThisLValue))
				{
					touch->UnSubscribe(sharedThisLValue);
					return;
				}
				m_processingTouch = touch;
				_SetTouchPressedState(true);
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				if (m_processingTouch)
				{
					UICoordinateType moveSquaredLength = (m_processingTouch->GetCurrentTouchPosition() - m_processingTouch->GetTouchStartPosition()).SquareLength();
					if (moveSquaredLength >= (m_moveThreshold * m_moveThreshold))
					{
						std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
						m_processingTouch->UnSubscribe(sharedThisLValue);
						m_processingTouch.reset();
						_SetTouchPressedState(false);
						return;
					}
				}
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
				touch->UnSubscribe(sharedThisLValue);
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
				touch->UnSubscribe(sharedThisLValue);
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				m_processingTouch.reset();
				if (!m_isPressed)
				{
					return;
				}

				_SetTouchPressedState(false);
				if (!touch->IsOwnPrimary(shared_from_this()))
				{
					return;
				}

				if (m_isLocked)
				{
					return;
				}
				m_onCancelHandlerFunc(std::dynamic_pointer_cast<CancelPanelArea>(shared_from_this()));
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
			{
				std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
				touch->UnSubscribe(sharedThisLValue);
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void CancelPanelArea::_SetTouchPressedState(bool isPressed)
			{
				if (m_isPressed == isPressed)
				{
					return;
				}
				m_isPressed = isPressed;
			}
		}
	}
}
