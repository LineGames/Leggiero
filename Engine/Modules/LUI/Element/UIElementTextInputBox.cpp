////////////////////////////////////////////////////////////////////////////////
// Element/UIElementTextInputBox.cpp (Leggiero/Modules - LegacyUI)
//
// Text Input Box Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementTextInputBox.h"

// Leggiero.LegacyUI
#include "../UIManager.h"
#include "../Common/UICommonArea.h"
#include "../Component/UIClippingComponent.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Touch/UITouch.h"
#include "../Touch/ValuedTouchComponent.h"
#include "../Rendering/TextRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// UIElementTextInputBox

			// Dummy Functions
			UIElementTextInputBox::EventHandlerType UIElementTextInputBox::ms_dummyEventHandler([](std::shared_ptr<UIElementTextInputBox> _1, GameDataString _2) { });
			UIElementTextInputBox::TextInputFilterType UIElementTextInputBox::ms_dummyFilter([](std::shared_ptr<UIElementTextInputBox> _1, GameDataString input) { return input; });
			UIElementTextInputBox::TextInputValidatorType UIElementTextInputBox::ms_dummyValidator([](std::shared_ptr<UIElementTextInputBox> _1, GameDataString _2) { return true; });

			//------------------------------------------------------------------------------
			UIElementTextInputBox::UIElementTextInputBox(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager, nullptr, true)
				, m_onTextInputHandlerFunc(ms_dummyEventHandler), m_onTextInputValidationFailedHandlerFunc(ms_dummyEventHandler)
				, m_inputFilter(ms_dummyFilter), m_inputValidator(ms_dummyValidator)
				, m_isPressed(false)
			{
			}

			//------------------------------------------------------------------------------
			UIElementTextInputBox::UIElementTextInputBox(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height,
				TypesettingGeneratorType typesettingGeneratorFunc, Graphics::GLByteARGB textMultiplyColor, const GameDataString &initialString,
				const GameDataString &dialogTitle, const GameDataString &dialogMessage, const GameDataString &dialogAcceptButton, const GameDataString &dialogCancelButton,
				EventHandlerType onTextInput, EventHandlerType onValidationFailed, TextInputFilterType textInputFilter, TextInputValidatorType textInputValidator,
				std::shared_ptr<RenderingController> renderingController)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)))
				, m_width(width), m_height(height), m_touchWidthRatio(static_cast<UICoordinateRatioType>(1.0f)), m_touchHeightRatio(static_cast<UICoordinateRatioType>(1.0f))
				, m_typesettingGeneratorFunc(typesettingGeneratorFunc), m_currentStringValue(initialString)
				, m_inputDialogTitleText(dialogTitle), m_inputDialogMessageText(dialogMessage), m_inputDialogAcceptButtonText(dialogAcceptButton), m_inputDialogCancelButtonText(dialogCancelButton)
				, m_onTextInputHandlerFunc(onTextInput), m_onTextInputValidationFailedHandlerFunc(onValidationFailed)
				, m_inputFilter(textInputFilter), m_inputValidator(textInputValidator)
				, m_renderingController(renderingController)
				, m_isPressed(false)
				, m_isClippingBySize(true)
			{
				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)));
				m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementTextInputBox::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&UIElementTextInputBox::_OnTouchIn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&UIElementTextInputBox::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&UIElementTextInputBox::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementTextInputBox::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementTextInputBox::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementTextInputBox::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);

				// Clipping Object
				m_textClippingSubObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))));
				m_textClippingRectComponent = std::make_shared<UIRectangularClipping>(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), width, height);
				m_textClippingSubObject->RegisterComponent(m_textClippingRectComponent);
				AddPostUIObject(m_textClippingSubObject);

				// Text Object
				m_textRenderingComponent = std::make_shared<Rendering::TextRenderingComponent>(nullptr);
				m_textSubObject = std::make_shared<TextUIObject>(ownerManager, m_textRenderingComponent);
				m_textSubObject->ChangeLayoutComponent(std::make_shared<UILayoutComponent>(std::make_shared<UIObjectSizeSubComponent>(m_textSubObject)));
				m_textClippingSubObject->AddPostUIObject(m_textSubObject);
				m_textRenderingComponent->SetGlobalMultiplyColor(textMultiplyColor);
			}

			//------------------------------------------------------------------------------
			UIElementTextInputBox::UIElementTextInputBox(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height,
				UICoordinateType clippingRectLeft, UICoordinateType clippingRectTop, UICoordinateType clippingRectWidth, UICoordinateType clippingRectHeight,
				TypesettingGeneratorType typesettingGeneratorFunc, Graphics::GLByteARGB textMultiplyColor, const GameDataString &initialString,
				const GameDataString &dialogTitle, const GameDataString &dialogMessage, const GameDataString &dialogAcceptButton, const GameDataString &dialogCancelButton,
				EventHandlerType onTextInput, EventHandlerType onValidationFailed, TextInputFilterType textInputFilter, TextInputValidatorType textInputValidator,
				std::shared_ptr<RenderingController> renderingController)
				: UIObject(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)))
				, m_width(width), m_height(height), m_touchWidthRatio(static_cast<UICoordinateRatioType>(1.0f)), m_touchHeightRatio(static_cast<UICoordinateRatioType>(1.0f))
				, m_typesettingGeneratorFunc(typesettingGeneratorFunc), m_currentStringValue(initialString)
				, m_inputDialogTitleText(dialogTitle), m_inputDialogMessageText(dialogMessage), m_inputDialogAcceptButtonText(dialogAcceptButton), m_inputDialogCancelButtonText(dialogCancelButton)
				, m_onTextInputHandlerFunc(onTextInput), m_onTextInputValidationFailedHandlerFunc(onValidationFailed)
				, m_inputFilter(textInputFilter), m_inputValidator(textInputValidator)
				, m_renderingController(renderingController)
				, m_isPressed(false)
				, m_isClippingBySize(false)
			{
				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(width, height)));
				m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->parentReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(0.5f); m_touchVirtualObject->GetLayoutComponent()->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(0.5f);
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementTextInputBox::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&UIElementTextInputBox::_OnTouchIn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&UIElementTextInputBox::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&UIElementTextInputBox::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementTextInputBox::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementTextInputBox::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementTextInputBox::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);

				// Clipping Object
				m_textClippingSubObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f))));
				m_textClippingRectComponent = UIRectangularClipping::CreateByPositionSize(clippingRectLeft, clippingRectTop, clippingRectWidth, clippingRectHeight);
				m_textClippingSubObject->RegisterComponent(m_textClippingRectComponent);
				AddPostUIObject(m_textClippingSubObject);

				// Text Object
				m_textRenderingComponent = std::make_shared<Rendering::TextRenderingComponent>(nullptr);
				m_textSubObject = std::make_shared<TextUIObject>(ownerManager, m_textRenderingComponent);
				m_textSubObject->ChangeLayoutComponent(std::make_shared<UILayoutComponent>(std::make_shared<UIObjectSizeSubComponent>(m_textSubObject)));
				m_textClippingSubObject->AddPostUIObject(m_textSubObject);
				m_textRenderingComponent->SetGlobalMultiplyColor(textMultiplyColor);
			}

			//------------------------------------------------------------------------------
			UIElementTextInputBox::~UIElementTextInputBox()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementTextInputBox::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<UIElementTextInputBox> clonedObject(new UIElementTextInputBox(managerCopy));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// My Data
				clonedObject->m_width = m_width;
				clonedObject->m_height = m_height;

				clonedObject->m_currentStringValue = m_currentStringValue;

				clonedObject->m_inputDialogTitleText = m_inputDialogTitleText;
				clonedObject->m_inputDialogMessageText = m_inputDialogMessageText;
				clonedObject->m_inputDialogAcceptButtonText = m_inputDialogAcceptButtonText;
				clonedObject->m_inputDialogCancelButtonText = m_inputDialogCancelButtonText;

				// Touch Area Virtual Object
				clonedObject->m_touchVirtualObject = std::make_shared<UIObject>(managerCopy, nullptr, true);
				clonedObject->m_touchVirtualObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_touchVirtualObject->GetLayoutComponent()->Clone(*clonedObject->m_touchVirtualObject)));
				clonedObject->m_touchVirtualObject->SetIsVirtualLogicObject(true);
				clonedObject->AddPostUIObject(clonedObject->m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				UIElementTextInputBox *rawPointer = clonedObject.get();
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject));
				touchComponent->SetCoverArea(std::make_shared<CommonArea::ObjectBasedArea>(clonedObject->m_touchVirtualObject));
				touchComponent->SetTouchDownFunc(std::bind(&UIElementTextInputBox::_OnTouchDown, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchInFunc(std::bind(&UIElementTextInputBox::_OnTouchIn, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
				touchComponent->SetTouchOutFunc(std::bind(&UIElementTextInputBox::_OnTouchOut, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&UIElementTextInputBox::_OnTouchCovered, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementTextInputBox::_OnTouchCancel, rawPointer, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementTextInputBox::_OnTouchUp, rawPointer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementTextInputBox::_OnPrimaryTouchLose, rawPointer, std::placeholders::_1));
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
			void UIElementTextInputBox::_UpdateFrameSelf(GameTimeClockType::duration frameInterval)
			{
				std::shared_ptr<RenderingController> controllerCopy(m_renderingController);
				if (controllerCopy)
				{
					StateType currentState = StateType::kNormal;
					if (m_isPressed)
					{
						currentState |= StateType::kPushed;
					}
					if (!IsEnable())
					{
						currentState |= StateType::kDisabled;
					}
					controllerCopy->UpdateFrameSelf(frameInterval, currentState, m_currentStringValue);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::GraphicPrepare()
			{
				if (m_typesetting && !m_isDirty)
				{
					return;
				}
				m_typesetting = m_typesettingGeneratorFunc(m_currentStringValue);
				m_isDirty = false;
				m_currentString = m_currentStringValue;
				m_textRenderingComponent->SetTypesetting(m_typesetting);
				m_textSubObject->m_effectiveTypesetting = m_typesetting;
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping)
			{
				if (m_isDirty || !m_typesetting)
				{
					m_typesetting = m_typesettingGeneratorFunc(m_currentStringValue);
					m_isDirty = false;
					m_currentString = m_currentStringValue;
					m_textRenderingComponent->SetTypesetting(m_typesetting);
					m_textSubObject->m_effectiveTypesetting = m_typesetting;
				}

				std::shared_ptr<RenderingController> controllerCopy(m_renderingController);
				if (controllerCopy)
				{
					controllerCopy->FrameGraphicPrepare(renderer, offsetPosition, effectiveClipping);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetRenderingController(std::shared_ptr<RenderingController> controller)
			{
				m_renderingController = controller;
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetString(const GameDataString &stringToSet)
			{
				if (m_isDirty)
				{
					m_currentStringValue = stringToSet;
				}
				else
				{
					if (m_currentString == stringToSet)
					{
						return;
					}
					m_isDirty = true;
					m_currentStringValue = stringToSet;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetTextMultiplyColor(Graphics::GLByteARGB multiplyColor)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->SetGlobalMultiplyColor(multiplyColor);
			}

			//------------------------------------------------------------------------------
			const Graphics::GLByteARGB UIElementTextInputBox::GetTextMultiplyColor() const
			{
				if (!m_textRenderingComponent)
				{
					return Graphics::GLByteARGB::kTransparent;
				}
				return m_textRenderingComponent->GetGlobalMultiplyColor();
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetWidth(UICoordinateType width)
			{
				m_width = width;
				std::dynamic_pointer_cast<UIValuedSizeSubComponent>(m_touchVirtualObject->GetLayoutComponent()->GetSizeSubComponent())->SetWidth((UICoordinateType)(width * m_touchWidthRatio));
				if (m_isClippingBySize)
				{
					m_textClippingRectComponent->SetWidth(width);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetHeight(UICoordinateType height)
			{
				m_height = height;
				std::dynamic_pointer_cast<UIValuedSizeSubComponent>(m_touchVirtualObject->GetLayoutComponent()->GetSizeSubComponent())->SetHeight((UICoordinateType)(height * m_touchHeightRatio));
				if (m_isClippingBySize)
				{
					m_textClippingRectComponent->SetHeight(height);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetTouchWidthRatio(UICoordinateRatioType ratio)
			{
				m_touchWidthRatio = ratio;
				std::dynamic_pointer_cast<UIValuedSizeSubComponent>(m_touchVirtualObject->GetLayoutComponent()->GetSizeSubComponent())->SetWidth((UICoordinateType)(m_width * m_touchWidthRatio));
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::SetTouchHeightRatio(UICoordinateRatioType ratio)
			{
				m_touchHeightRatio = ratio;
				std::dynamic_pointer_cast<UIValuedSizeSubComponent>(m_touchVirtualObject->GetLayoutComponent()->GetSizeSubComponent())->SetHeight((UICoordinateType)(m_height * m_touchHeightRatio));
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIRectangularClipping> UIElementTextInputBox::GetRawTextClippingComponent(bool isProhibitClippingBySize)
			{
				if (isProhibitClippingBySize)
				{
					m_isClippingBySize = false;
				}
				return m_textClippingRectComponent;
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::InitializeAdjustClippingLayoutAsBox()
			{
				m_textClippingSubObject->ChangeLayoutComponent(std::make_shared<UILayoutComponent>(std::make_shared<UIObjectSizeSubComponent>(shared_from_this())));
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
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
			void UIElementTextInputBox::_OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant)
			{
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchPressedState(true);
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchPressedState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
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

				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return;
				}

				managerCopy->RequestOpenInputDialog(std::bind(&UIElementTextInputBox::_OnTextInputFinished, this, std::placeholders::_1, std::placeholders::_2),
					m_inputDialogTitleText.c_str(), m_inputDialogMessageText.c_str(), "", m_inputDialogAcceptButtonText.c_str(), m_inputDialogCancelButtonText.c_str()
				);
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
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
			void UIElementTextInputBox::_SetTouchPressedState(bool isPressed)
			{
				if (m_isPressed == isPressed)
				{
					return;
				}
				m_isPressed = isPressed;
			}

			//------------------------------------------------------------------------------
			void UIElementTextInputBox::_OnTextInputFinished(bool isSuccessed, GameDataString inputText)
			{
				if (!isSuccessed)
				{
					return;
				}

				std::shared_ptr<UIElementTextInputBox> sharedThis(std::dynamic_pointer_cast<UIElementTextInputBox>(shared_from_this()));

				GameDataString filteredText(m_inputFilter(sharedThis, inputText));
				bool isValidInput = m_inputValidator(sharedThis, filteredText);

				if (!isValidInput)
				{
					m_onTextInputValidationFailedHandlerFunc(sharedThis, filteredText);
					return;
				}

				SetString(filteredText);

				m_onTextInputHandlerFunc(sharedThis, m_currentStringValue);
			}


			//////////////////////////////////////////////////////////////////////////////// UIElementTextInputBox::TextUIObject

			//------------------------------------------------------------------------------
			UIElementTextInputBox::TextUIObject::TextUIObject(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<Rendering::TextRenderingComponent> renderingComponent)
				: UIObject(ownerManager, nullptr, true)
			{
				RegisterComponent(renderingComponent);
			}

			//------------------------------------------------------------------------------
			UIElementTextInputBox::TextUIObject::~TextUIObject()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementTextInputBox::TextUIObject::Clone() const
			{
				return nullptr;
			}

			//------------------------------------------------------------------------------
			UIElementSize UIElementTextInputBox::TextUIObject::CalculateSize()
			{
				if (!m_effectiveTypesetting)
				{
					return UIElementSize();
				}
				return UIElementSize(m_effectiveTypesetting->boundingBox.GetWidth(), m_effectiveTypesetting->logicalHeight);
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementTextInputBox::TextUIObject::CalculateWidth()
			{
				if (!m_effectiveTypesetting)
				{
					return static_cast<UICoordinateType>(0.0f);
				}
				return (UICoordinateType)m_effectiveTypesetting->boundingBox.GetWidth();
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementTextInputBox::TextUIObject::CalculateHeight()
			{
				if (!m_effectiveTypesetting)
				{
					return static_cast<UICoordinateType>(0.0f);
				}
				return (UICoordinateType)m_effectiveTypesetting->logicalHeight;
			}
		}
	}
}
