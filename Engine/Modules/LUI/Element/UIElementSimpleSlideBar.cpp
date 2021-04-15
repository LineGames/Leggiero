////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSimpleSliderBar.cpp (Leggiero/Modules - LegacyUI)
//
// Simple Slider Bar Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementSimpleSlideBar.h"

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
			//////////////////////////////////////////////////////////////////////////////// UIElementSimpleSlideBar

			// Dummy Value Changed Handler Function
			UIElementSimpleSlideBar::OnValueChangedHandlerType UIElementSimpleSlideBar::ms_dummyOnValueChangedHandler([](std::shared_ptr<UIElementSimpleSlideBar> _1, double _2) { });
			UIElementSimpleSlideBar::OnTouchStateChangedHandlerType UIElementSimpleSlideBar::ms_dummyOnTouchStateChangedHandler([](std::shared_ptr<UIElementSimpleSlideBar> _1, bool _2) { });

			//------------------------------------------------------------------------------
			UIElementSimpleSlideBar::UIElementSimpleSlideBar(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager)
				, m_onValueChangedHandlerFunc(ms_dummyOnValueChangedHandler)
				, m_currentValue(0.0), m_isPressed(false)
				, m_onTouchStateChangedHandlerFunc(ms_dummyOnTouchStateChangedHandler)
				, m_subRulerDivide(0)
				, m_isHoldPrimaryOnControlling(true)
			{
			}

			//------------------------------------------------------------------------------
			UIElementSimpleSlideBar::UIElementSimpleSlideBar(std::shared_ptr<UIManager> ownerManager, UICoordinateType width,
				std::shared_ptr<UIObject> marker, UICoordinateRatioType markerInteractionWidthRatio, UICoordinateRatioType markerInteractionHeightRatio,
				int subRulerDivide, double initialValue, OnValueChangedHandlerType onValueChanged,
				std::shared_ptr<RenderingController> renderingController)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, 0.0f)))
				, m_width(width)
				, m_markerInteractionWidthRatio(markerInteractionWidthRatio), m_markerInteractionHeightRatio(markerInteractionHeightRatio)
				, m_markerObject(marker)
				, m_onValueChangedHandlerFunc(onValueChanged), m_subRulerDivide(subRulerDivide), m_currentValue(initialValue)
				, m_renderingController(renderingController)
				, m_isPressed(false)
				, m_onTouchStateChangedHandlerFunc(ms_dummyOnTouchStateChangedHandler)
				, m_isHoldPrimaryOnControlling(true)
			{
				if (marker)
				{
					m_postChildren.push_back(marker);
				}

				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIObjectSizeSubComponent>(marker)));
				m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject, markerInteractionWidthRatio, markerInteractionHeightRatio));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchMoveFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchStealFunc(std::bind(&UIElementSimpleSlideBar::_OnPrimaryTouchSteal, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementSimpleSlideBar::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);

				_UpdateMarkerPosition();
			}

			//------------------------------------------------------------------------------
			UIElementSimpleSlideBar::~UIElementSimpleSlideBar()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementSimpleSlideBar::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<UIElementSimpleSlideBar> clonedObject(new UIElementSimpleSlideBar(managerCopy));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// My Data
				clonedObject->m_width = m_width;

				clonedObject->m_markerInteractionWidthRatio = m_markerInteractionWidthRatio;
				clonedObject->m_markerInteractionHeightRatio = m_markerInteractionHeightRatio;

				clonedObject->m_markerObject = m_markerObject->Clone();
				clonedObject->m_postChildren.push_back(m_markerObject);

				clonedObject->m_subRulerDivide = m_subRulerDivide;

				clonedObject->m_currentValue = m_currentValue;

				clonedObject->m_onValueChangedHandlerFunc = m_onValueChangedHandlerFunc;
				clonedObject->m_onTouchStateChangedHandlerFunc = m_onTouchStateChangedHandlerFunc;

				clonedObject->m_isHoldPrimaryOnControlling = m_isHoldPrimaryOnControlling;

				// Touch Area Virtual Object
				clonedObject->m_touchVirtualObject = std::make_shared<UIObject>(managerCopy, std::make_shared<UILayoutComponent>(std::make_shared<UIObjectSizeSubComponent>(clonedObject->m_markerObject)));
				clonedObject->m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); clonedObject->m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				clonedObject->m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				clonedObject->m_touchVirtualObject->SetIsVirtualLogicObject(true);
				clonedObject->AddPostUIObject(clonedObject->m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				UIElementSimpleSlideBar *rawPointer = clonedObject.get();
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject, m_markerInteractionWidthRatio, m_markerInteractionHeightRatio));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchDown, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchMoveFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchMove, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchCancel, rawPointer, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementSimpleSlideBar::_OnTouchUp, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementSimpleSlideBar::_OnPrimaryTouchLose, rawPointer, std::placeholders::_1));
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
					if (m_markerObject && currentChild == m_markerObject)
					{
						continue;
					}
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

				clonedObject->_UpdateMarkerPosition();

				return clonedObject;
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_UpdateFrameSelf(GameTimeClockType::duration frameInterval)
			{
				std::shared_ptr<RenderingController> controllerCopy(m_renderingController);
				if (controllerCopy)
				{
					controllerCopy->UpdateFrameSelf(frameInterval, m_isPressed);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping)
			{
				std::shared_ptr<RenderingController> controllerCopy(m_renderingController);
				if (controllerCopy)
				{
					controllerCopy->FrameGraphicPrepare(renderer, offsetPosition, effectiveClipping);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::SetRenderingController(std::shared_ptr<RenderingController> controller)
			{
				m_renderingController = controller;
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::ForceSetValue(double value, bool isSuppressNotify, bool isForceNotify)
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
					_SetTouchPressedState(false);
					m_processingTouch.reset();
				}

				if (m_subRulerDivide > 0)
				{
					double multipliedValue = value * (double)m_subRulerDivide;
					value = ((double)round(multipliedValue) / (double)m_subRulerDivide);
				}

				if (value < 0.0)
				{
					value = 0.0;
				}
				else if (value > 1.0)
				{
					value = 1.0;
				}

				if (!isSuppressNotify)
				{
					bool toNotify = false;
					if (value != m_currentValue)
					{
						toNotify = true;
						m_currentValue = value;
					}
					else if (isForceNotify)
					{
						toNotify = true;
					}

					if (toNotify)
					{
						m_onValueChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleSlideBar>(shared_from_this()), value);
					}
				}
				else
				{
					m_currentValue = value;
				}

				_UpdateMarkerPosition();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				if (m_processingTouch)
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
				m_touchStartValue = m_currentValue;
				_SetTouchPressedState(true);
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_UpdateTouchedValue();
				_UpdateMarkerPosition();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_UpdateTouchedValue();
				_UpdateMarkerPosition();

				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			bool UIElementSimpleSlideBar::_OnPrimaryTouchSteal(std::shared_ptr<UITouch> touch, std::shared_ptr<UIObject> stealer)
			{
				if (!m_isHoldPrimaryOnControlling || !m_isPressed)
				{
					return true;
				}
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return true;
				}

				return false;
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
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
			void UIElementSimpleSlideBar::_SetTouchPressedState(bool isPressed)
			{
				if (m_isPressed == isPressed)
				{
					return;
				}
				m_isPressed = isPressed;
				m_onTouchStateChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleSlideBar>(shared_from_this()), isPressed);
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_UpdateMarkerPosition()
			{
				if (m_markerObject)
				{
					m_markerObject->GetLayoutComponent()->parentOffsetX = (UICoordinateType)(m_width * m_currentValue);
				}
				m_touchVirtualObject->GetLayoutComponent()->parentOffsetX = (UICoordinateType)(m_width * m_currentValue);
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::_UpdateTouchedValue()
			{
				if (!m_processingTouch)
				{
					return;
				}

				UICoordinateType delta = m_processingTouch->GetCurrentTouchX() - m_processingTouch->GetTouchStartX();
				double changeAmount = delta / m_width;
				double realValue = m_touchStartValue + changeAmount;

				if (m_subRulerDivide > 0)
				{
					double multipliedValue = realValue * (double)m_subRulerDivide;
					realValue = ((double)round(multipliedValue) / (double)m_subRulerDivide);
				}

				if (realValue < 0.0)
				{
					realValue = 0.0;
				}
				else if (realValue > 1.0)
				{
					realValue = 1.0;
				}

				bool toNotify = false;
				if (realValue != m_currentValue)
				{
					toNotify = true;
					m_currentValue = realValue;
				}

				if (toNotify)
				{
					m_onValueChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleSlideBar>(shared_from_this()), realValue);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleSlideBar::SetSubRulerDivide(int subRulerDevide, bool isSuppressNotify)
			{
				if (subRulerDevide <= 0)
				{
					m_subRulerDivide = 0;
					return;
				}
				m_subRulerDivide = subRulerDevide;

				double prevValue = m_currentValue;
				double multipliedValue = m_currentValue * (double)subRulerDevide;
				m_currentValue = ((double)round(multipliedValue) / (double)subRulerDevide);

				if (!isSuppressNotify && prevValue != m_currentValue)
				{
					m_onValueChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleSlideBar>(shared_from_this()), m_currentValue);
				}
			}
		}
	}
}
