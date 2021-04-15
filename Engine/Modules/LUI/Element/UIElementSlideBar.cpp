////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSlideBar.cpp (Leggiero/Modules - LegacyUI)
//
// Slide bar Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementSlideBar.h"

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
			//////////////////////////////////////////////////////////////////////////////// UIElementSlideBar

			// Dummy Value Changed Handler Function
			UIElementSlideBar::OnValueChangedHandlerType UIElementSlideBar::ms_dummyOnValueChangedHandler([](std::shared_ptr<UIElementSlideBar> _1, double _2) { });

			namespace UIElementSlideBarUtil
			{
				constexpr UICoordinateRatioType kMarkerLineLengthMultiplier = (UICoordinateRatioType)1.8f;
				constexpr UICoordinateRatioType kMarkerExtendRatio = (UICoordinateRatioType)3.2f;
			}

			//------------------------------------------------------------------------------
			UIElementSlideBar::UIElementSlideBar(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager)
				, m_onValueChangedHandlerFunc(ms_dummyOnValueChangedHandler)
				, m_currentValue(0.0), m_isPressed(false)
			{
			}

			//------------------------------------------------------------------------------
			UIElementSlideBar::UIElementSlideBar(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height, UICoordinateType barLineWidth, Graphics::GLByteARGB barColor,
				UICoordinateType markerSize, UICoordinateType markerLineWidth, Graphics::GLByteARGB markerColor,
				OnValueChangedHandlerType onValueChanged, int subRulerDivide, double initialValue,
				bool isChangeMarkerOnPressed, UICoordinateType markerSizeOnPressed, UICoordinateType markerLineWidthOnPressed, Graphics::GLByteARGB markerColorOnPressed)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)))
				, m_width(width), m_height(height), m_barLineWidth(barLineWidth), m_barColor(barColor)
				, m_markerSize(markerSize), m_markerLineWidth(markerLineWidth), m_markerColor(markerColor)
				, m_onValueChangedHandlerFunc(onValueChanged), m_subRulerDivide(subRulerDivide), m_currentValue(initialValue)
				, m_isChangeMarkerOnPressed(isChangeMarkerOnPressed), m_markerSizeOnPressed(markerSizeOnPressed), m_markerLineWidthOnPressed(markerLineWidthOnPressed), m_markerColorOnPressed(markerColorOnPressed)
				, m_isPressed(false)
			{
				// Initialize Slide Bar
				m_barShapes.push_back(std::make_shared<UIShapeRect>(width + barLineWidth, barLineWidth, barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(barLineWidth * 0.5f), static_cast<UICoordinateType>(height * 0.5f - barLineWidth * 0.5f)));
				m_barShapes.push_back(std::make_shared<UIShapeRect>(barLineWidth, height, barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(barLineWidth * 0.5f), static_cast<UICoordinateType>(0.0f)));
				m_barShapes.push_back(std::make_shared<UIShapeRect>(barLineWidth, height, barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, static_cast<UICoordinateType>(width - barLineWidth * 0.5f), static_cast<UICoordinateType>(0.0f)));
				if (subRulerDivide > 1)
				{
					for (int i = 1; i < subRulerDivide; ++i)
					{
						double rulerValue = (double)i / (double)subRulerDivide;
						m_barShapes.push_back(std::make_shared<UIShapeRect>(barLineWidth, height, barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, static_cast<UICoordinateType>(rulerValue * width - barLineWidth * 0.5f), static_cast<UICoordinateType>(0.0f)));
					}
				}
				std::shared_ptr<Rendering::ShapeSetRenderingComponent> barRenderingComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>());
				for (std::shared_ptr<UIShapeRect> &currentBarElement : m_barShapes)
				{
					barRenderingComponent->AddShape(currentBarElement);
				}
				RegisterComponent(barRenderingComponent);

				// Markers
				m_markerShapes.push_back(std::make_shared<UIShapeRect>(markerSize, markerSize, Graphics::GLByteARGB::kTransparent, markerLineWidth, markerColor, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(markerSize * 0.5f), -static_cast<UICoordinateType>(markerSize * 0.5f)));
				m_markerShapes.push_back(std::make_shared<UIShapeRect>(markerLineWidth, static_cast<UICoordinateType>(markerSize * UIElementSlideBarUtil::kMarkerLineLengthMultiplier), markerColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(markerLineWidth * 0.5f), -static_cast<UICoordinateType>(markerSize * UIElementSlideBarUtil::kMarkerLineLengthMultiplier * 0.5f)));
				m_markerMainObject = std::make_shared<UIObject>(ownerManager);
				m_markerMainObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{ m_markerShapes[0], m_markerShapes[1] }));
				m_markerMainObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				AddPostUIObject(m_markerMainObject);
				m_pressedMarkerMainObject = std::make_shared<UIObject>(ownerManager);
				if (m_isChangeMarkerOnPressed)
				{
					m_pressedMarkerShapes.push_back(std::make_shared<UIShapeRect>(markerSizeOnPressed, markerSizeOnPressed, Graphics::GLByteARGB::kTransparent, markerLineWidthOnPressed, markerColorOnPressed, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(markerSizeOnPressed * 0.5f), -static_cast<UICoordinateType>(markerSizeOnPressed * 0.5f)));
					m_pressedMarkerShapes.push_back(std::make_shared<UIShapeRect>(markerLineWidthOnPressed, static_cast<UICoordinateType>(markerSizeOnPressed * UIElementSlideBarUtil::kMarkerLineLengthMultiplier), markerColorOnPressed, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(markerLineWidthOnPressed * 0.5f), -static_cast<UICoordinateType>(markerSizeOnPressed * UIElementSlideBarUtil::kMarkerLineLengthMultiplier * 0.5f)));
					m_pressedMarkerMainObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
					m_pressedMarkerMainObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{ m_pressedMarkerShapes[0], m_pressedMarkerShapes[1] }));
					m_pressedMarkerMainObject->SetIsVisible(false);
					AddPostUIObject(m_pressedMarkerMainObject);
				}

				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(markerSize, markerSize)));
				m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject, UIElementSlideBarUtil::kMarkerExtendRatio, UIElementSlideBarUtil::kMarkerExtendRatio));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementSlideBar::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchMoveFunc(std::bind(&UIElementSlideBar::_OnTouchMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementSlideBar::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementSlideBar::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementSlideBar::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);

				_UpdateMarkerPosition();
			}

			//------------------------------------------------------------------------------
			UIElementSlideBar::~UIElementSlideBar()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementSlideBar::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<UIElementSlideBar> clonedObject(new UIElementSlideBar(managerCopy));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// My Data
				clonedObject->m_width = m_width;
				clonedObject->m_height = m_height;
				clonedObject->m_barLineWidth = m_barLineWidth;
				clonedObject->m_barColor = m_barColor;

				clonedObject->m_markerSize = m_markerSize;
				clonedObject->m_markerLineWidth = m_markerLineWidth;
				clonedObject->m_markerColor = m_markerColor;

				clonedObject->m_isChangeMarkerOnPressed = m_isChangeMarkerOnPressed;
				clonedObject->m_markerSizeOnPressed = m_markerSizeOnPressed;
				clonedObject->m_markerLineWidthOnPressed = m_markerLineWidthOnPressed;
				clonedObject->m_markerColorOnPressed = m_markerColorOnPressed;

				clonedObject->m_subRulerDivide = m_subRulerDivide;

				clonedObject->m_currentValue = m_currentValue;

				// Initialize Slide Bar
				clonedObject->m_barShapes.push_back(std::make_shared<UIShapeRect>(m_width + m_barLineWidth, m_barLineWidth, m_barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(m_barLineWidth * 0.5f), static_cast<UICoordinateType>(m_height * 0.5f - m_barLineWidth * 0.5f)));
				clonedObject->m_barShapes.push_back(std::make_shared<UIShapeRect>(m_barLineWidth, m_height, m_barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(m_barLineWidth * 0.5f), static_cast<UICoordinateType>(0.0f)));
				clonedObject->m_barShapes.push_back(std::make_shared<UIShapeRect>(m_barLineWidth, m_height, m_barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, static_cast<UICoordinateType>(m_width - m_barLineWidth * 0.5f), static_cast<UICoordinateType>(0.0f)));
				if (m_subRulerDivide > 1)
				{
					for (int i = 1; i < m_subRulerDivide; ++i)
					{
						double rulerValue = (double)i / (double)m_subRulerDivide;
						clonedObject->m_barShapes.push_back(std::make_shared<UIShapeRect>(m_barLineWidth, m_height, m_barColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, static_cast<UICoordinateType>(rulerValue * m_width - m_barLineWidth * 0.5f), static_cast<UICoordinateType>(0.0f)));
					}
				}
				std::shared_ptr<Rendering::ShapeSetRenderingComponent> barRenderingComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>());
				for (std::shared_ptr<UIShapeRect> &currentBarElement : clonedObject->m_barShapes)
				{
					barRenderingComponent->AddShape(currentBarElement);
				}
				clonedObject->RegisterComponent(barRenderingComponent);

				// Markers
				clonedObject->m_markerShapes.push_back(std::make_shared<UIShapeRect>(m_markerSize, m_markerSize, Graphics::GLByteARGB::kTransparent, m_markerLineWidth, m_markerColor, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(m_markerSize * 0.5f), -static_cast<UICoordinateType>(m_markerSize * 0.5f)));
				clonedObject->m_markerShapes.push_back(std::make_shared<UIShapeRect>(m_markerLineWidth, static_cast<UICoordinateType>(m_markerSize * UIElementSlideBarUtil::kMarkerLineLengthMultiplier), m_markerColor, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(m_markerLineWidth * 0.5f), -static_cast<UICoordinateType>(m_markerSize * UIElementSlideBarUtil::kMarkerLineLengthMultiplier * 0.5f)));
				clonedObject->m_markerMainObject = std::make_shared<UIObject>(managerCopy);
				clonedObject->m_markerMainObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{ clonedObject->m_markerShapes[0], clonedObject->m_markerShapes[1] }));
				clonedObject->m_markerMainObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				clonedObject->AddPostUIObject(clonedObject->m_markerMainObject);
				clonedObject->m_pressedMarkerMainObject = std::make_shared<UIObject>(managerCopy);
				if (m_isChangeMarkerOnPressed)
				{
					clonedObject->m_pressedMarkerShapes.push_back(std::make_shared<UIShapeRect>(m_markerSizeOnPressed, m_markerSizeOnPressed, Graphics::GLByteARGB::kTransparent, m_markerLineWidthOnPressed, m_markerColorOnPressed, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(m_markerSizeOnPressed * 0.5f), -static_cast<UICoordinateType>(m_markerSizeOnPressed * 0.5f)));
					clonedObject->m_pressedMarkerShapes.push_back(std::make_shared<UIShapeRect>(m_markerLineWidthOnPressed, static_cast<UICoordinateType>(m_markerSizeOnPressed * UIElementSlideBarUtil::kMarkerLineLengthMultiplier), m_markerColorOnPressed, static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB::kTransparent, ShapeBorderAlignType::kCenter, -static_cast<UICoordinateType>(m_markerLineWidthOnPressed * 0.5f), -static_cast<UICoordinateType>(m_markerSizeOnPressed * UIElementSlideBarUtil::kMarkerLineLengthMultiplier * 0.5f)));
					clonedObject->m_pressedMarkerMainObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
					clonedObject->m_pressedMarkerMainObject->RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{ clonedObject->m_pressedMarkerShapes[0], clonedObject->m_pressedMarkerShapes[1] }));
					clonedObject->m_pressedMarkerMainObject->SetIsVisible(false);
					clonedObject->AddPostUIObject(clonedObject->m_pressedMarkerMainObject);
				}

				// Touch Area Virtual Object
				clonedObject->m_touchVirtualObject = std::make_shared<UIObject>(managerCopy, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(m_markerSize, m_markerSize)));
				clonedObject->m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); clonedObject->m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				clonedObject->m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				clonedObject->m_touchVirtualObject->SetIsVirtualLogicObject(true);
				clonedObject->AddPostUIObject(clonedObject->m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				UIElementSlideBar *rawPointer = clonedObject.get();
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject, UIElementSlideBarUtil::kMarkerExtendRatio, UIElementSlideBarUtil::kMarkerExtendRatio));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementSlideBar::_OnTouchDown, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchMoveFunc(std::bind(&UIElementSlideBar::_OnTouchMove, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementSlideBar::_OnTouchCancel, rawPointer, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementSlideBar::_OnTouchUp, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementSlideBar::_OnPrimaryTouchLose, rawPointer, std::placeholders::_1));
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
					if (m_markerMainObject && currentChild == m_markerMainObject)
					{
						continue;
					}
					if (m_pressedMarkerMainObject && currentChild == m_pressedMarkerMainObject)
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
			void UIElementSlideBar::SetBarColor(Graphics::GLByteARGB barColor)
			{
				m_barColor = barColor;
				for (std::shared_ptr<UIShapeRect> &currentBarElement : m_barShapes)
				{
					currentBarElement->SetColor(barColor);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::SetMarkerColor(Graphics::GLByteARGB markerColor, Graphics::GLByteARGB markerColorOnPressed)
			{
				m_markerColor = markerColor;
				m_barShapes[0]->SetBorderColor(markerColor);
				m_barShapes[1]->SetColor(markerColor);

				m_markerColorOnPressed = markerColorOnPressed;
				if (m_isChangeMarkerOnPressed)
				{
					m_pressedMarkerShapes[0]->SetBorderColor(markerColorOnPressed);
					m_pressedMarkerShapes[1]->SetColor(markerColorOnPressed);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::ForceSetValue(double value)
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
					_SetTouchPressedState(false);
					m_processingTouch.reset();
				}

				if (value < 0.0)
				{
					value = 0.0;
				}
				else if (value > 1.0)
				{
					value = 1.0;
				}

				bool toNotify = false;
				if (value != m_currentValue)
				{
					toNotify = true;
					m_currentValue = value;
				}

				if (toNotify)
				{
					m_onValueChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSlideBar>(shared_from_this()), value);
				}

				_UpdateMarkerPosition();
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
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
			void UIElementSlideBar::_OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_UpdateTouchedValue();
				_UpdateMarkerPosition();
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_UpdateTouchedValue();
				_UpdateMarkerPosition();

				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
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
			void UIElementSlideBar::_SetTouchPressedState(bool isPressed)
			{
				if (m_isPressed == isPressed)
				{
					return;
				}
				m_isPressed = isPressed;

				_UpdatePressedGraphic();
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_UpdatePressedGraphic()
			{
				if (!m_isChangeMarkerOnPressed)
				{
					return;
				}

				if (m_isPressed)
				{
					m_markerMainObject->SetIsVisible(false);
					m_pressedMarkerMainObject->SetIsVisible(true);
				}
				else
				{
					m_markerMainObject->SetIsVisible(true);
					m_pressedMarkerMainObject->SetIsVisible(false);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_UpdateMarkerPosition()
			{
				m_markerMainObject->GetLayoutComponent()->parentOffsetX = static_cast<UICoordinateType>(m_width * m_currentValue);
				m_pressedMarkerMainObject->GetLayoutComponent()->parentOffsetX = static_cast<UICoordinateType>(m_width * m_currentValue);
				m_touchVirtualObject->GetLayoutComponent()->parentOffsetX = static_cast<UICoordinateType>(m_width * m_currentValue);
			}

			//------------------------------------------------------------------------------
			void UIElementSlideBar::_UpdateTouchedValue()
			{
				if (!m_processingTouch)
				{
					return;
				}

				UICoordinateType delta = m_processingTouch->GetCurrentTouchX() - m_processingTouch->GetTouchStartX();
				double changeAmount = delta / m_width;
				double realValue = m_touchStartValue + changeAmount;
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
					m_onValueChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSlideBar>(shared_from_this()), realValue);
				}
			}
		}
	}
}
