////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSingleSimpleButton.cpp (Leggiero/Modules - LegacyUI)
//
// Simple button element implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementSingleSimpleButton.h"

// Leggiero.LegacyUI
#include "../Common/UICommonArea.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Rendering/ShapeSetRenderingComponent.h"
#include "../Rendering/UIShapeRect.h"
#include "../Touch/ValuedTouchComponent.h"
#include "../Touch/UITouch.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// SingleSimpleButton

			// Dummy Click Handler Function
			SingleSimpleButton::OnClickHandlerType SingleSimpleButton::ms_dummyOnClickHandler([](std::shared_ptr<SingleSimpleButton> _1) { });

			//------------------------------------------------------------------------------
			SingleSimpleButton::SingleSimpleButton(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager, nullptr, true)
				, m_onClickHandlerFunc(ms_dummyOnClickHandler)
				, m_isPressed(false), m_isActive(false)
			{
			}

			//------------------------------------------------------------------------------
			SingleSimpleButton::SingleSimpleButton(std::shared_ptr<UIManager> ownerManager, UICoordinateType touchWidth, UICoordinateType touchHeight,
				OnClickHandlerType onClick,
				std::shared_ptr<RenderingController> renderingController)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(touchWidth, touchHeight)))
				, m_width(touchWidth), m_height(touchHeight), m_coverWidth(touchWidth), m_coverHeight(touchHeight)
				, m_onClickHandlerFunc(onClick), m_renderingController(renderingController)
				, m_isPressed(false), m_isActive(false)
			{
				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(touchWidth, touchHeight)));
				m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&SingleSimpleButton::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&SingleSimpleButton::_OnTouchIn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&SingleSimpleButton::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&SingleSimpleButton::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&SingleSimpleButton::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&SingleSimpleButton::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&SingleSimpleButton::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);
			}

			//------------------------------------------------------------------------------
			SingleSimpleButton::SingleSimpleButton(std::shared_ptr<UIManager> ownerManager, UICoordinateType touchWidth, UICoordinateType touchHeight, UICoordinateType touchCoverWidth, UICoordinateType touchCoverHeight,
				OnClickHandlerType onClick,
				std::shared_ptr<RenderingController> renderingController)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(touchWidth, touchHeight)))
				, m_width(touchWidth), m_height(touchHeight), m_coverWidth(touchCoverWidth), m_coverHeight(touchCoverHeight)
				, m_onClickHandlerFunc(onClick), m_renderingController(renderingController)
				, m_isPressed(false), m_isActive(false)
			{
				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(touchWidth, touchHeight)));
				m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject, touchCoverWidth / touchWidth, touchCoverHeight / touchHeight));
				touchComponent->SetTouchDownFunc(std::bind(&SingleSimpleButton::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&SingleSimpleButton::_OnTouchIn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&SingleSimpleButton::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&SingleSimpleButton::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&SingleSimpleButton::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&SingleSimpleButton::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&SingleSimpleButton::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);
			}

			//------------------------------------------------------------------------------
			SingleSimpleButton::~SingleSimpleButton()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> SingleSimpleButton::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<SingleSimpleButton> clonedObject(new SingleSimpleButton(managerCopy));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// My Data
				clonedObject->m_width = m_width;
				clonedObject->m_height = m_height;

				clonedObject->m_coverWidth = m_coverWidth;
				clonedObject->m_coverHeight = m_coverHeight;

				// Touch Area Virtual Object
				clonedObject->m_touchVirtualObject = std::make_shared<UIObject>(managerCopy, nullptr, true);
				clonedObject->m_touchVirtualObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_touchVirtualObject->GetLayoutComponent()->Clone(*clonedObject->m_touchVirtualObject)));
				clonedObject->m_touchVirtualObject->SetIsVirtualLogicObject(true);
				clonedObject->AddPostUIObject(clonedObject->m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				SingleSimpleButton *rawPointer = clonedObject.get();
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject, m_coverWidth / m_width, m_coverHeight / m_height));
				touchComponent->SetTouchDownFunc(std::bind(&SingleSimpleButton::_OnTouchDown, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&SingleSimpleButton::_OnTouchIn, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&SingleSimpleButton::_OnTouchOut, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&SingleSimpleButton::_OnTouchCovered, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&SingleSimpleButton::_OnTouchCancel, rawPointer, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&SingleSimpleButton::_OnTouchUp, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&SingleSimpleButton::_OnPrimaryTouchLose, rawPointer, std::placeholders::_1));
				clonedObject->RegisterComponent(touchComponent);

				std::unordered_set<UIComponentType, Utility::SyntacticSugar::EnumClassHash> copiedComponentSet;
				copiedComponentSet.insert(UIComponentType::kRendering);
				copiedComponentSet.insert(UIComponentType::kTouchInteraction);
				while (copiedComponentSet.size() < m_components.size())
				{
					bool isCopiedLoop = false;
					for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::const_iterator it = m_components.begin(); it != m_components.end(); ++it)
					{
						if (copiedComponentSet.find(it->first) != copiedComponentSet.end())
						{
							continue;
						}

						const std::vector<UIComponentType> &dependencyList = it->second->GetDependentComponentTypes();
						bool isUnResolvedDependencyExists = false;
						for (const UIComponentType &dependingComponentType : dependencyList)
						{
							if (copiedComponentSet.find(dependingComponentType) == copiedComponentSet.end())
							{
								isUnResolvedDependencyExists = true;
								break;
							}
						}
						if (isUnResolvedDependencyExists)
						{
							continue;
						}

						isCopiedLoop = true;
						copiedComponentSet.insert(it->first);
						std::shared_ptr<IUIComponent> clonedComponent(it->second->Clone(*clonedObject));
						if (clonedComponent)
						{
							clonedObject->m_components[it->first] = clonedComponent;
						}
					}
					if (!isCopiedLoop)
					{
						// Cyclic Component Dependency Exists
						break;
					}
				}

				clonedObject->m_preChildren.reserve(m_preChildren.size());
				for (const std::shared_ptr<UIObject> &currentChild : m_preChildren)
				{
					std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
					if (clonedChild)
					{
						clonedChild->UpdateParent(*clonedObject);
						clonedObject->m_preChildren.push_back((clonedChild));
					}
				}
				clonedObject->m_postChildren.reserve(m_postChildren.size());
				for (const std::shared_ptr<UIObject> &currentChild : m_postChildren)
				{
					if (m_touchVirtualObject && currentChild == m_touchVirtualObject)
					{
						continue;
					}
					std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
					if (clonedChild)
					{
						clonedChild->UpdateParent(*clonedObject);
						clonedObject->m_postChildren.push_back((clonedChild));
					}
				}

				return clonedObject;
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_UpdateFrameSelf(GameTimeClockType::duration frameInterval)
			{
				std::shared_ptr<RenderingController> controllerCopy(m_renderingController);
				if (controllerCopy)
				{
					StateType currentState = StateType::kNormal;
					if (m_isPressed)
					{
						currentState |= StateType::kPushed;
					}
					if (m_isActive)
					{
						currentState |= StateType::kActive;
					}
					if (!IsEnable())
					{
						currentState |= StateType::kDisabled;
					}
					controllerCopy->UpdateFrameSelf(frameInterval, currentState);
				}
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping)
			{
				std::shared_ptr<RenderingController> controllerCopy(m_renderingController);
				if (controllerCopy)
				{
					controllerCopy->FrameGraphicPrepare(renderer, offsetPosition, effectiveClipping);
				}
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::SetRenderingController(std::shared_ptr<RenderingController> controller)
			{
				m_renderingController = controller;
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::SetTouchWidth(UICoordinateType width)
			{
				m_width = width;
				std::dynamic_pointer_cast<UIValuedSizeSubComponent>(m_touchVirtualObject->GetLayoutComponent()->GetSizeSubComponent())->SetWidth(width);
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::SetTouchHeight(UICoordinateType height)
			{
				m_height = height;
				std::dynamic_pointer_cast<UIValuedSizeSubComponent>(m_touchVirtualObject->GetLayoutComponent()->GetSizeSubComponent())->SetHeight(height);
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				if (m_processingTouch)
				{
					return;
				}
				if (isCoveredByDescendant && touch->IsPrimaryOccupied())
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
			void SingleSimpleButton::_OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant)
			{
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchPressedState(true);
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
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

				if (!IsEnable())
				{
					return;
				}
				m_onClickHandlerFunc(std::dynamic_pointer_cast<SingleSimpleButton>(shared_from_this()));
			}

			//------------------------------------------------------------------------------
			void SingleSimpleButton::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
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
			void SingleSimpleButton::_SetTouchPressedState(bool isPressed)
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
