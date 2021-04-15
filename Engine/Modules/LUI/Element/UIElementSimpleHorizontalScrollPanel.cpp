////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSimpleHorizontalScrollPanel.cpp (Leggiero/Modules - LegacyUI)
//
// Horizontal Scroll Panel Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementSimpleHorizontalScrollPanel.h"

// Leggiero.LegacyUI
#include "../Common/UICommonArea.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Component/UIClippingComponent.h"
#include "../Touch/ValuedTouchComponent.h"
#include "../Touch/UITouch.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// UIElementSimpleHorizontalScrollPanel

			// Dummy Click Handler Function
			UIElementSimpleHorizontalScrollPanel::OnPositionChangeHandlerType UIElementSimpleHorizontalScrollPanel::ms_dummyOnPositionChangeHandler([](std::shared_ptr<UIElementSimpleHorizontalScrollPanel> _1, UIElementSimpleHorizontalScrollPanel::PositionValueType _2) { });

			//------------------------------------------------------------------------------
			UIElementSimpleHorizontalScrollPanel::UIElementSimpleHorizontalScrollPanel(std::shared_ptr<UIManager> ownerManager)
				: UIObject(ownerManager, nullptr, true)
				, m_onPositionChangedHandlerFunc(ms_dummyOnPositionChangeHandler)
				, m_isTouchedMoving(false), m_currentPosition(static_cast<PositionValueType>(0.0f)), m_currentInertiaSpeed(static_cast<PositionValueType>(0.0f)), m_isRecoverToBoundary(false), m_speedDataSampleCount(0), m_isScrolling(false)
				, m_isCullingSubObjects(true)
			{
			}

			//------------------------------------------------------------------------------
			UIElementSimpleHorizontalScrollPanel::UIElementSimpleHorizontalScrollPanel(std::shared_ptr<UIManager> ownerManager, UICoordinateType panelHeight,
				UICoordinateType boundedWidth, std::shared_ptr<UIObject> contentsObject, UICoordinateType contentsWidth,
				UICoordinateType scrollMovingThreshold, bool isClipping,
				OnPositionChangeHandlerType onPositionChanged)
				: UIObject(ownerManager, nullptr, true)
				, m_height(panelHeight), m_boundedWidth(boundedWidth), m_contentsObject(contentsObject), m_contentsWidth(contentsWidth), m_isScrolling(true)
				, m_scrollMovingThreshold(scrollMovingThreshold)
				, m_onPositionChangedHandlerFunc(onPositionChanged)
				, m_isTouchedMoving(false), m_currentPosition(static_cast<PositionValueType>(0.0f)), m_currentInertiaSpeed(static_cast<PositionValueType>(0.0f)), m_isRecoverToBoundary(false), m_speedDataSampleCount(0)
				, m_isCullingSubObjects(true)
			{
				// Make Own Layout
				m_layoutComponent = std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(boundedWidth, panelHeight));

				// Touch Area Virtual Object
				m_touchVirtualObject = std::make_shared<UIObject>(ownerManager, std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(boundedWidth, panelHeight)));
				m_touchVirtualObject->SetIsVirtualLogicObject(true);
				AddPostUIObject(m_touchVirtualObject);

				// Touch Handler
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(m_touchVirtualObject));
				touchComponent->SetCoverArea(touchComponent->GetInteractionArea());
				touchComponent->SetTouchDownFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchMoveFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnPrimaryTouchLose, this, std::placeholders::_1));
				RegisterComponent(touchComponent);

				// Initialize Values
				m_maximumValue = contentsWidth - boundedWidth;
				if (m_maximumValue <= 0.0f)
				{
					m_maximumValue = 0.0f;
					m_isScrolling = false;
				}

				// Set Clipping
				if (isClipping)
				{
					RegisterComponent(std::make_shared<UIRectangularClipping>(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), m_boundedWidth, panelHeight));
				}
			}

			//------------------------------------------------------------------------------
			UIElementSimpleHorizontalScrollPanel::~UIElementSimpleHorizontalScrollPanel()
			{
				if (m_processingTouch)
				{
					std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
					m_processingTouch->UnSubscribe(sharedThisLValue);
				}
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementSimpleHorizontalScrollPanel::Clone() const
			{
				std::shared_ptr<UIObject> clonedObject(UIObject::Clone());
				std::shared_ptr<UIElementSimpleHorizontalScrollPanel> concreteCloned(std::dynamic_pointer_cast<UIElementSimpleHorizontalScrollPanel>(clonedObject));
				if (!concreteCloned)
				{
					return clonedObject;
				}

				concreteCloned->m_boundedWidth = m_boundedWidth;
				concreteCloned->m_contentsWidth = m_contentsWidth;

				concreteCloned->m_currentPosition = m_currentPosition;
				concreteCloned->m_maximumValue = m_maximumValue;
				concreteCloned->m_isScrolling = m_isScrolling;

				concreteCloned->m_scrollMovingThreshold = m_scrollMovingThreshold;

				concreteCloned->m_onPositionChangedHandlerFunc = m_onPositionChangedHandlerFunc;

				// Substitute Touch Virtual Object
				concreteCloned->UnRegisterComponent(UIComponentType::kTouchInteraction);
				concreteCloned->RemoveUIObject(concreteCloned->m_touchVirtualObject);
				concreteCloned->m_touchVirtualObject = std::make_shared<UIObject>(m_ownerManager.lock(), std::make_shared<UILayoutComponent>(std::make_shared<UIValuedSizeSubComponent>(m_height, m_boundedWidth)));
				concreteCloned->m_touchVirtualObject->SetIsVirtualLogicObject(true);
				concreteCloned->AddPostUIObject(concreteCloned->m_touchVirtualObject);
				std::shared_ptr<ValuedTouchComponent> touchComponent(std::make_shared<ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<CommonArea::ObjectBasedArea>(concreteCloned->m_touchVirtualObject));
				touchComponent->SetCoverArea(touchComponent->GetInteractionArea());
				touchComponent->SetTouchDownFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchDown, concreteCloned, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchMoveFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchMove, concreteCloned, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchCancel, concreteCloned, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnTouchUp, concreteCloned, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&UIElementSimpleHorizontalScrollPanel::_OnPrimaryTouchLose, concreteCloned, std::placeholders::_1));
				concreteCloned->RegisterComponent(touchComponent);

				return concreteCloned;
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_UpdateFrameSelf(GameTimeClockType::duration frameInterval)
			{
				constexpr PositionValueType kSmallValue = static_cast<PositionValueType>(0.005f);

				// Update Position
				if (!m_isTouchedMoving && m_isScrolling)
				{
					PositionValueType positionValueCopy = m_currentPosition;
					constexpr float kDampingOmega0 = 24.0f;
					if (m_isRecoverToBoundary)
					{
						m_accumulatedRecoverTime += static_cast<float>((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
						float expTerm = static_cast<float>(exp(-kDampingOmega0 * m_accumulatedRecoverTime));
						float currentDisplacement = static_cast<float>((m_recoverConstantA + m_accumulatedRecoverTime * m_recoverConstantB) * expTerm);
						if (fabs(currentDisplacement) < kSmallValue)
						{
							m_isRecoverToBoundary = false;
							m_currentInertiaSpeed = static_cast<PositionValueType>(0.0f);
							if (m_isRecoverToZero)
							{
								m_currentPosition = static_cast<PositionValueType>(0.0f);
							}
							else
							{
								m_currentPosition = GetMaximumValue();
							}
						}
						else
						{
							if (currentDisplacement < 0.0f)
							{
								m_currentInertiaSpeed = (float)(m_recoverConstantB * expTerm - m_recoverConstantA * kDampingOmega0 * expTerm - m_recoverConstantB * m_accumulatedRecoverTime * kDampingOmega0 * expTerm);
								if (m_currentInertiaSpeed < -kSmallValue)
								{
									m_isRecoverToBoundary = false;
									if (m_isRecoverToZero)
									{
										m_currentInertiaSpeed = -m_currentInertiaSpeed;
									}
								}
							}

							if (m_isRecoverToZero)
							{
								m_currentPosition = -currentDisplacement;
							}
							else
							{
								m_currentPosition = GetMaximumValue() + currentDisplacement;
							}
						}
					}
					else
					{
						PositionValueType maxValue = GetMaximumValue();
						if (m_currentPosition < static_cast<PositionValueType>(0.0f))
						{
							m_isRecoverToBoundary = true;
							m_accumulatedRecoverTime = 0.0f;
							m_isRecoverToZero = true;
							m_recoverConstantA = -m_currentPosition;
							m_recoverConstantB = -m_currentInertiaSpeed - kDampingOmega0 * m_currentPosition;
						}
						else if (m_currentPosition > maxValue)
						{
							m_isRecoverToBoundary = true;
							m_accumulatedRecoverTime = 0.0f;
							m_isRecoverToZero = false;
							m_recoverConstantA = m_currentPosition - maxValue;
							m_recoverConstantB = m_currentInertiaSpeed + kDampingOmega0 * (m_currentPosition - maxValue);
						}
						else if (fabs(m_currentInertiaSpeed) > kSmallValue)
						{
							float frameElapsedSec = static_cast<float>((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
							m_currentPosition += m_currentInertiaSpeed * frameElapsedSec;
							m_currentInertiaSpeed *= static_cast<float>(pow(0.94, frameElapsedSec * 60.0f));
							if (fabs(m_currentInertiaSpeed) < kSmallValue)
							{
								m_currentInertiaSpeed = static_cast<PositionValueType>(0.0f);
							}
						}
						else
						{
							m_currentInertiaSpeed = static_cast<PositionValueType>(0.0f);
						}
					}

					// Position Changed by Inertial Moving
					if (positionValueCopy != m_currentPosition)
					{
						m_onPositionChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleHorizontalScrollPanel>(shared_from_this()), m_currentPosition);
					}
				}

				// Set UI Position
				if (m_isScrolling)
				{
					UICoordinateType offsetValue = static_cast<UICoordinateType>(-m_currentPosition);
					offsetValue = static_cast<UICoordinateType>(floor(offsetValue));
					m_contentsObject->GetLayoutComponent()->parentOffsetX = offsetValue;
				}
				else
				{
					m_contentsObject->GetLayoutComponent()->parentOffsetX = static_cast<UICoordinateType>(0.0f);
				}

				// Cull Sub-Objects
				if (m_isCullingSubObjects)
				{
					UICoordinateType minimumValue = static_cast<UICoordinateType>(m_currentPosition - 1.0f);
					UICoordinateType maximumValue = static_cast<UICoordinateType>(m_currentPosition + m_boundedWidth + 1.0f);
					for (std::shared_ptr<UIObject> &currentChild : m_contentsObject->GetPreChildren())
					{
						std::shared_ptr<UILayoutComponent> childLayoutComponentHolder(currentChild->GetLayoutComponent());
						UILayoutComponent &childLayoutComponent = *childLayoutComponentHolder;
						UIElementSize childSize = childLayoutComponent.GetSizeSubComponent()->CalculateSize();

						UICoordinateType childLeft = static_cast<UICoordinateType>(childLayoutComponent.parentReferenceRatioX * m_contentsWidth + childLayoutComponent.parentOffsetX - childLayoutComponent.anchorReferenceRatioX * childSize.width - childLayoutComponent.anchorOffsetX);
						UICoordinateType childRight = static_cast<UICoordinateType>(childLeft + childSize.width);

						currentChild->SetIsVisible(!(childLeft > maximumValue || childRight < minimumValue));
					}
					for (std::shared_ptr<UIObject> &currentChild : m_contentsObject->GetPostChildren())
					{
						std::shared_ptr<UILayoutComponent> childLayoutComponentHolder(currentChild->GetLayoutComponent());
						UILayoutComponent &childLayoutComponent = *childLayoutComponentHolder;
						UIElementSize childSize = childLayoutComponent.GetSizeSubComponent()->CalculateSize();

						UICoordinateType childTop = static_cast<UICoordinateType>(childLayoutComponent.parentReferenceRatioX * m_contentsWidth + childLayoutComponent.parentOffsetX - childLayoutComponent.anchorReferenceRatioX * childSize.width - childLayoutComponent.anchorOffsetX);
						UICoordinateType childBottom = static_cast<UICoordinateType>(childTop + childSize.width);

						currentChild->SetIsVisible(!(childTop > maximumValue || childBottom < minimumValue));
					}
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::SetPosition(UIElementSimpleHorizontalScrollPanel::PositionValueType positionValue, bool isNotifyToHandler)
			{
				if (m_processingTouch)
				{
					_SetTouchedMovingState(false);
					m_processingTouch.reset();
				}
				Stop();

				m_currentPosition = positionValue;
				if (isNotifyToHandler)
				{
					m_onPositionChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleHorizontalScrollPanel>(shared_from_this()), positionValue);
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::Stop()
			{
				m_currentInertiaSpeed = static_cast<PositionValueType>(0.0f);
				m_isRecoverToBoundary = false;
			}

			//------------------------------------------------------------------------------
			UIElementSimpleHorizontalScrollPanel::PositionValueType UIElementSimpleHorizontalScrollPanel::GetMaximumValue() const
			{
				return m_maximumValue;
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::SetContentsWidth(UICoordinateType width)
			{
				Stop();
				m_contentsWidth = width;

				m_maximumValue = m_contentsWidth - m_boundedWidth;
				if (m_maximumValue <= 0.0f)
				{
					m_maximumValue = 0.0f;
					m_isScrolling = false;
				}
				else
				{
					m_isScrolling = true;
				}

				if (m_currentPosition > m_maximumValue)
				{
					m_currentPosition = m_maximumValue;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				if (!m_isScrolling)
				{
					return;
				}
				if (m_processingTouch)
				{
					return;
				}
				std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
				touch->Subscribe(sharedThisLValue, true);
				if (!touch->IsPrimaryOccupied())
				{
					if (!touch->ClaimPrimary(sharedThisLValue))
					{
						touch->UnSubscribe(sharedThisLValue);
						return;
					}
					m_processingTouch = touch;
					_SetTouchedMovingState(true);
				}
				else
				{
					m_processingTouch = touch;
				}
				m_touchStartPosition = m_currentPosition;

				Stop();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				if (!m_isTouchedMoving)
				{
					UICoordinateType delta = m_processingTouch->GetCurrentTouchX() - m_processingTouch->GetTouchStartX();
					if (static_cast<UICoordinateType>(fabs(delta)) >= m_scrollMovingThreshold)
					{
						std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
						if (!touch->ClaimPrimary(sharedThisLValue))
						{
							return;
						}
						_SetTouchedMovingState(true);
					}
					else
					{
						return;
					}
				}

				_UpdateTouchScrolledValue();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time)
			{
				_SetTouchedMovingState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time)
			{
				if (!m_isTouchedMoving)
				{
					m_processingTouch.reset();
					return;
				}

				_UpdateTouchScrolledValue();

				_SetTouchedMovingState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_OnPrimaryTouchLose(std::shared_ptr<UITouch> touch)
			{
				std::shared_ptr<UIObject> sharedThisLValue(shared_from_this());
				touch->UnSubscribe(sharedThisLValue);
				if (!m_processingTouch || (touch != m_processingTouch))
				{
					return;
				}
				_SetTouchedMovingState(false);
				m_processingTouch.reset();
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_SetTouchedMovingState(bool isTouchedMoving)
			{
				if (m_isTouchedMoving == isTouchedMoving)
				{
					return;
				}
				m_isTouchedMoving = isTouchedMoving;
				if (isTouchedMoving)
				{
					m_speedDataSampleCount = 0;
					m_lastTouchProcessTime = m_processingTouch->GetCurrentTouchChangeTime();
				}
				else if (m_isScrolling)
				{
					m_isRecoverToBoundary = false;
					if (m_speedDataSampleCount > 0)
					{
						float timeSum = 0.0f;
						float moveSum = 0.0f;
						for (int i = 0; i < m_speedDataSampleCount; ++i)
						{
							timeSum += m_speedDataSample[i].elapsedTime;
							moveSum += m_speedDataSample[i].moveAmount;
						}
						if (timeSum < kFloatEpsilon)
						{
							// Divide by zero
							m_currentInertiaSpeed = 0.0f;
						}
						else
						{
							m_currentInertiaSpeed = static_cast<PositionValueType>(moveSum / timeSum);
						}
					}
					else
					{
						m_currentInertiaSpeed = 0.0f;
					}
				}
				else
				{
					m_isRecoverToBoundary = false;
					m_currentInertiaSpeed = 0.0f;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_UpdateTouchScrolledValue()
			{
				if (!m_processingTouch)
				{
					return;
				}

				constexpr float kOverBoundaryMoveRatio = 0.32f;
				constexpr float kOverBoundaryMoveRevertMultiplier = (1.0f - kOverBoundaryMoveRatio);

				UICoordinateType delta = m_processingTouch->GetCurrentTouchX() - m_processingTouch->GetTouchStartX();
				PositionValueType changeAmount = -static_cast<PositionValueType>(delta);

				PositionValueType nextPosition = m_touchStartPosition + changeAmount;
				PositionValueType maxValue = GetMaximumValue();
				if (nextPosition < static_cast<PositionValueType>(0.0f))
				{
					if (m_touchStartPosition < static_cast<PositionValueType>(0.0f))
					{
						if (changeAmount < static_cast<PositionValueType>(0.0f))
						{
							nextPosition -= static_cast<PositionValueType>(changeAmount * kOverBoundaryMoveRevertMultiplier);
						}
					}
					else
					{
						nextPosition -= static_cast<PositionValueType>((changeAmount + m_touchStartPosition) * kOverBoundaryMoveRevertMultiplier);
					}
				}
				else if (nextPosition > maxValue)
				{
					if (m_touchStartPosition > maxValue)
					{
						if (changeAmount > static_cast<PositionValueType>(0.0f))
						{
							nextPosition -= static_cast<PositionValueType>(changeAmount * kOverBoundaryMoveRevertMultiplier);
						}
					}
					else
					{
						nextPosition -= static_cast<PositionValueType>((changeAmount - (maxValue - m_touchStartPosition)) * kOverBoundaryMoveRevertMultiplier);
					}
				}

				GameTimeClockType::duration touchProcessInterval(m_processingTouch->GetCurrentTouchChangeTime() - m_lastTouchProcessTime);
				_AppendSpeedSampleData(static_cast<float>((float)std::chrono::duration_cast<std::chrono::microseconds>(touchProcessInterval).count() / 1000000.0), nextPosition - m_currentPosition);
				m_lastTouchProcessTime = m_processingTouch->GetCurrentTouchChangeTime();

				bool toNotify = false;
				if (nextPosition != m_currentPosition)
				{
					toNotify = true;
					m_currentPosition = nextPosition;
				}

				if (toNotify)
				{
					m_onPositionChangedHandlerFunc(std::dynamic_pointer_cast<UIElementSimpleHorizontalScrollPanel>(shared_from_this()), nextPosition);
				}
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementSimpleHorizontalScrollPanel::_AllocateCloningObject() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}
				return std::shared_ptr<UIObject>(dynamic_cast<UIObject *>(new UIElementSimpleHorizontalScrollPanel(managerCopy)));
			}

			//------------------------------------------------------------------------------
			void UIElementSimpleHorizontalScrollPanel::_AppendSpeedSampleData(float elapsedTime, float moveAmount)
			{
				if (elapsedTime < kFloatEpsilon)
				{
					// Ignore Invalid Value
					return;
				}

				if (m_speedDataSampleCount >= kInertiaSpeedSampleFrame)
				{
					for (int i = 0; i < kInertiaSpeedSampleFrame - 1; ++i)
					{
						m_speedDataSample[i] = m_speedDataSample[i + 1];
					}
				}
				else
				{
					++m_speedDataSampleCount;
				}

				m_speedDataSample[m_speedDataSampleCount - 1].elapsedTime = elapsedTime;
				m_speedDataSample[m_speedDataSampleCount - 1].moveAmount = moveAmount;
			}
		}
	}
}
