////////////////////////////////////////////////////////////////////////////////
// Element/UIElementTestButton.cpp (Leggiero/Modules - LegacyUI)
//
// Test button element implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementTestButton.h"

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
			//////////////////////////////////////////////////////////////////////////////// UIElementTestButton

			// Dummy Click Handler Function
			UIElementTestButton::OnClickHandlerType UIElementTestButton::ms_dummyOnClickHandler([](std::shared_ptr<UIElementTestButton> _1) { });

			namespace UIElementTestButtonUtil
			{
				constexpr UICoordinateType kOffsetX = static_cast<UICoordinateType>(8.0f);
				constexpr UICoordinateType kOffsetY = static_cast<UICoordinateType>(8.0f);

				constexpr UICoordinateRatioType kExpendedAreaMarginHorizontalRatio = (UICoordinateRatioType)1.24f;
				constexpr UICoordinateRatioType kExpendedAreaMarginVerticalRatio = (UICoordinateRatioType)1.36f;
			}

			//------------------------------------------------------------------------------
			UIElementTestButton::UIElementTestButton(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager, nullptr, true)
				, m_onClickHandlerFunc(ms_dummyOnClickHandler)
				, m_isPressed(false)
			{
			}

			//------------------------------------------------------------------------------
			UIElementTestButton::UIElementTestButton(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height, Graphics::GLByteARGB color,
				UICoordinateType borderWidth, Graphics::GLByteARGB borderColor,
				std::shared_ptr<UIObject> buttonContent, OnClickHandlerType onClick,
				bool isChangeColorOnPressed, Graphics::GLByteARGB colorOnPressed, Graphics::GLByteARGB borderColorOnPressed)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)))
				, m_width(width), m_height(height), m_buttonColor(color), m_buttonBorderWidth(borderWidth), m_buttonBorderColor(borderColor)
				, m_buttonContent(buttonContent), m_onClickHandlerFunc(onClick)
				, m_isChangeColorOnPressed(isChangeColorOnPressed), m_colorOnPressed(colorOnPressed), m_borderColorOnPressed(borderColorOnPressed)
				, m_isPressed(false)
				, m_isClickGraphicInvert(false)
			{
				// Button Shadow
				RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{
					std::make_shared<UIShapeRect>(width, height, Graphics::GLByteARGB(216, 16, 16, 16))
				}));

				// Button Main Content
				m_buttonMainRectShape = std::make_shared<UIShapeRect>(width, height, color, borderWidth, borderColor, ShapeBorderAlignType::kInside);
				m_buttonMainObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)));
				m_buttonMainObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(m_buttonMainRectShape));
				if (buttonContent)
				{
					m_buttonMainObject->AddPostUIObject(buttonContent);
				}
				m_buttonMainObject->GetLayoutComponent()->anchorOffsetX = UIElementTestButtonUtil::kOffsetX; m_buttonMainObject->GetLayoutComponent()->anchorOffsetY = UIElementTestButtonUtil::kOffsetY;
				AddPostUIObject(m_buttonMainObject);

				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width + UIElementTestButtonUtil::kOffsetX, height + UIElementTestButtonUtil::kOffsetY)));
				m_touchVirtualObject->GetLayoutComponent()->anchorOffsetX = UIElementTestButtonUtil::kOffsetX; m_touchVirtualObject->GetLayoutComponent()->anchorOffsetY = UIElementTestButtonUtil::kOffsetY;
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject, UIElementTestButtonUtil::kExpendedAreaMarginHorizontalRatio, UIElementTestButtonUtil::kExpendedAreaMarginVerticalRatio));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_buttonMainObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementTestButton::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&UIElementTestButton::_OnTouchIn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&UIElementTestButton::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&UIElementTestButton::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementTestButton::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementTestButton::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementTestButton::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);
			}

			//------------------------------------------------------------------------------
			UIElementTestButton::~UIElementTestButton()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementTestButton::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<UIElementTestButton> clonedObject(new UIElementTestButton(managerCopy));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// My Data
				clonedObject->m_width = m_width;
				clonedObject->m_height = m_height;

				clonedObject->m_buttonColor = m_buttonColor;
				clonedObject->m_buttonBorderWidth = m_buttonBorderWidth;
				clonedObject->m_buttonBorderColor = m_buttonBorderColor;

				clonedObject->m_isChangeColorOnPressed = m_isChangeColorOnPressed;
				clonedObject->m_colorOnPressed = m_colorOnPressed;
				clonedObject->m_borderColorOnPressed = m_borderColorOnPressed;

				clonedObject->m_onClickHandlerFunc = m_onClickHandlerFunc;
				clonedObject->m_isClickGraphicInvert = m_isClickGraphicInvert;

				clonedObject->m_buttonContent = m_buttonContent ? m_buttonContent->Clone() : nullptr;

				// Button Shadow
				clonedObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{
					std::make_shared<UIShapeRect>(m_width, m_height, Graphics::GLByteARGB(216, 16, 16, 16))
				}));

				// Button Main Content
				clonedObject->m_buttonMainRectShape = std::make_shared<UIShapeRect>(m_width, m_height, m_buttonColor, m_buttonBorderWidth, m_buttonBorderColor, ShapeBorderAlignType::kInside);
				clonedObject->m_buttonMainObject = std::make_shared<UIObject>(managerCopy, nullptr, true);
				clonedObject->m_buttonMainObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(clonedObject->m_buttonMainRectShape));
				if (clonedObject->m_buttonContent)
				{
					clonedObject->m_buttonMainObject->AddPostUIObject(clonedObject->m_buttonContent);
				}
				clonedObject->m_buttonMainObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_buttonMainObject->GetLayoutComponent()->Clone(*clonedObject->m_buttonMainObject)));
				clonedObject->AddPostUIObject(clonedObject->m_buttonMainObject);

				// Touch Area Virtual Object
				clonedObject->m_touchVirtualObject = std::make_shared<UIObject>(managerCopy, nullptr, true);
				clonedObject->m_touchVirtualObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_touchVirtualObject->GetLayoutComponent()->Clone(*clonedObject->m_touchVirtualObject)));
				clonedObject->m_touchVirtualObject->SetIsVirtualLogicObject(true);
				clonedObject->AddPostUIObject(clonedObject->m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				UIElementTestButton *rawPointer = clonedObject.get();
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject, UIElementTestButtonUtil::kExpendedAreaMarginHorizontalRatio, UIElementTestButtonUtil::kExpendedAreaMarginVerticalRatio));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_buttonMainObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementTestButton::_OnTouchDown, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&UIElementTestButton::_OnTouchIn, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&UIElementTestButton::_OnTouchOut, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&UIElementTestButton::_OnTouchCovered, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementTestButton::_OnTouchCancel, rawPointer, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementTestButton::_OnTouchUp, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementTestButton::_OnPrimaryTouchLose, rawPointer, std::placeholders::_1));
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
					if (m_buttonMainObject && currentChild == m_buttonMainObject)
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

				return clonedObject;
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
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
				_SetTouchPressedState(true);
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant)
			{
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchPressedState(true);
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
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
				m_onClickHandlerFunc(std::dynamic_pointer_cast<UIElementTestButton>(shared_from_this()));
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
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
			void UIElementTestButton::_SetTouchPressedState(bool isPressed)
			{
				if (m_isPressed == isPressed)
				{
					return;
				}
				m_isPressed = isPressed;

				_UpdatePressedGraphic();
			}

			//------------------------------------------------------------------------------
			void UIElementTestButton::_UpdatePressedGraphic()
			{
				bool effectivePressed = (m_isPressed != m_isClickGraphicInvert);
				if (!IsEnable())
				{
					effectivePressed = !m_isClickGraphicInvert;
				}

				m_buttonMainObject->GetLayoutComponent()->anchorOffsetX = effectivePressed ? static_cast<UICoordinateType>(0.0f) : UIElementTestButtonUtil::kOffsetX;
				m_buttonMainObject->GetLayoutComponent()->anchorOffsetY = effectivePressed ? static_cast<UICoordinateType>(0.0f) : UIElementTestButtonUtil::kOffsetY;

				if (m_isChangeColorOnPressed)
				{
					if (effectivePressed)
					{
						m_buttonMainRectShape->SetColor(m_colorOnPressed);
						m_buttonMainRectShape->SetBorderColor(m_borderColorOnPressed);
					}
					else
					{
						m_buttonMainRectShape->SetColor(m_buttonColor);
						m_buttonMainRectShape->SetBorderColor(m_buttonBorderColor);
					}
				}
			}
		}
	}
}
