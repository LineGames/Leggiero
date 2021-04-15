////////////////////////////////////////////////////////////////////////////////
// Touch/UITouch.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation of UI Touch object
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITouch.h"

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Component/UIComponentType.h"
#include "UITouchComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UITouch

		//------------------------------------------------------------------------------
		UITouch::UITouch(Input::TouchIdType touchId, UICoordinateType startX, UICoordinateType startY, GameTimeClockType::time_point startTime)
			: m_isAlive(true), m_touchId(touchId)
			, m_startTime(startTime), m_startX(startX), m_startY(startY)
			, m_lastChangeTime(startTime), m_lastX(startX), m_lastY(startY)
			, m_currentChangeTime(startTime), m_currentX(startX), m_currentY(startY)
		{

		}

		//------------------------------------------------------------------------------
		UITouch::~UITouch()
		{

		}

		//------------------------------------------------------------------------------
		// Subscribe events from the touch.
		// While not subscribing, can only receive Down and In events.
		// When isStrong is false, OnTouchOut, OnTouchCovered event implicitly means un-subcribing.
		void UITouch::Subscribe(std::shared_ptr<UIObject> &subscriber, bool isStrong)
		{
			if (!subscriber)
			{
				return;
			}

			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (subscriber == pointerCopy)
					{
						if (std::get<0>(*it) == isStrong)
						{
							// Already Subscribed
							return;
						}

						m_subscribers.erase(it);
						break;
					}
				}
			}

			m_subscribers.push_back(std::tuple<bool, std::weak_ptr<UIObject> >(isStrong, std::weak_ptr<UIObject>(subscriber)));
		}

		//------------------------------------------------------------------------------
		// Un-subscribe events of the touch
		void UITouch::UnSubscribe(std::shared_ptr<UIObject> &subscriber)
		{
			if (!subscriber)
			{
				return;
			}

			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (subscriber == pointerCopy)
					{
						m_subscribers.erase(it);
						break;
					}
				}
			}
			if (m_primarySubscriber)
			{
				if (subscriber == m_primarySubscriber)
				{
					m_primarySubscriber.reset();
					{
						std::shared_ptr<UITouchComponent> touchComponent = subscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
						if (touchComponent)
						{
							touchComponent->OnPrimaryTouchLose(shared_from_this());
						}
					}
					for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
					{
						std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
						if (pointerCopy)
						{
							if (subscriber != pointerCopy)
							{
								std::shared_ptr<UITouchComponent> touchComponent = pointerCopy->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
								if (touchComponent)
								{
									touchComponent->OnPrimaryTouchHolderChanged(shared_from_this(), nullptr);
									if (m_primarySubscriber)
									{
										// Primary Subscriber Changed
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		// Request to be primary subscriber
		bool UITouch::ClaimPrimary(std::shared_ptr<UIObject> &subscriber)
		{
			if (!subscriber)
			{
				return false;
			}
			if (!IsSubscribed(subscriber))
			{
				return false;
			}

			if (m_primarySubscriber)
			{
				if (m_primarySubscriber == subscriber)
				{
					return true;
				}

				// Check Steal
				{
					std::shared_ptr<UITouchComponent> touchComponent = m_primarySubscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
					if (touchComponent)
					{
						if (!touchComponent->OnPrimaryTouchSteal(shared_from_this(), subscriber))
						{
							// Steal Refused
							return false;
						}
						m_primarySubscriber = nullptr;

						touchComponent->OnPrimaryTouchLose(shared_from_this());
					}
				}

				m_primarySubscriber = subscriber;
				for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
				{
					std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
					if (pointerCopy)
					{
						if (subscriber != pointerCopy)
						{
							std::shared_ptr<UITouchComponent> touchComponent = pointerCopy->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
							if (touchComponent)
							{
								touchComponent->OnPrimaryTouchHolderChanged(shared_from_this(), m_primarySubscriber);
								if (!m_primarySubscriber || (m_primarySubscriber != subscriber))
								{
									// Primary Subscriber Changed
									return false;
								}
							}
						}
					}
				}

				{
					std::shared_ptr<UITouchComponent> touchComponent = subscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
					if (touchComponent)
					{
						touchComponent->OnPrimaryTouchGiven(shared_from_this());
					}
				}
				return true;
			}
			else
			{
				m_primarySubscriber = subscriber;
				for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
				{
					std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
					if (pointerCopy)
					{
						if (subscriber != pointerCopy)
						{
							std::shared_ptr<UITouchComponent> touchComponent = pointerCopy->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
							if (touchComponent)
							{
								touchComponent->OnPrimaryTouchHolderChanged(shared_from_this(), m_primarySubscriber);
								if (!m_primarySubscriber || (m_primarySubscriber != subscriber))
								{
									// Primary Subscriber Changed
									return false;
								}
							}
						}
					}
				}

				{
					std::shared_ptr<UITouchComponent> touchComponent = subscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
					if (touchComponent)
					{
						touchComponent->OnPrimaryTouchGiven(shared_from_this());
					}
				}
				return true;
			}
		}

		//------------------------------------------------------------------------------
		// Resign primary subscriber
		void UITouch::ResignPrimary(std::shared_ptr<UIObject> &subscriber)
		{
			if (!subscriber)
			{
				return;
			}
			if (!m_primarySubscriber)
			{
				return;
			}
			if (m_primarySubscriber != subscriber)
			{
				return;
			}

			m_primarySubscriber.reset();
			{
				std::shared_ptr<UITouchComponent> touchComponent = subscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
				if (touchComponent)
				{
					touchComponent->OnPrimaryTouchLose(shared_from_this());
				}
			}
			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (subscriber != pointerCopy)
					{
						std::shared_ptr<UITouchComponent> touchComponent = pointerCopy->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
						if (touchComponent)
						{
							touchComponent->OnPrimaryTouchHolderChanged(shared_from_this(), nullptr);
							if (m_primarySubscriber)
							{
								// Primary Subscriber Changed
								break;
							}
						}
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		bool UITouch::IsSubscribed(const std::shared_ptr<UIObject> &subscriber) const
		{
			if (!subscriber)
			{
				return false;
			}

			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::const_iterator it = m_subscribers.cbegin(); it != m_subscribers.cend(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (subscriber == pointerCopy)
					{
						return true;
					}
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------
		bool UITouch::IsOwnPrimary(const std::shared_ptr<UIObject> &subscriber) const
		{
			if (!subscriber)
			{
				return false;
			}
			if (!m_primarySubscriber)
			{
				return false;
			}
			return (m_primarySubscriber == subscriber);
		}

		//------------------------------------------------------------------------------
		bool UITouch::IsPrimaryOccupied() const
		{
			return ((bool)m_primarySubscriber);
		}

		//------------------------------------------------------------------------------
		void UITouch::UpdateTouchDataUsingEvent(const Input::Touch::TouchEvent &touchEvent)
		{
			m_lastChangeTime = m_currentChangeTime;
			m_lastX = m_currentX;
			m_lastY = m_currentY;

			m_currentChangeTime = touchEvent.eventTime;
			if (Utility::SyntacticSugar::HasFlag(touchEvent.eventType, Input::Touch::TouchEventType::kTouchUpdatePosition))
			{
				m_currentX = static_cast<UICoordinateType>(touchEvent.x);
				m_currentY = static_cast<UICoordinateType>(touchEvent.y);
			}

			if (Utility::SyntacticSugar::HasFlag(touchEvent.eventType, Input::Touch::TouchEventType::kTouchEnded))
			{
				m_isAlive = false;
			}
		}

		//------------------------------------------------------------------------------
		void UITouch::_ProcessTouchMoveEvent(std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects)
		{
			std::shared_ptr<UIObject> processedPrimary(m_primarySubscriber);
			if (processedPrimary)
			{
				if (_NotifyMoveToStrongSubscriber(processedPrimary, allTouchState, outProcessedObjects, true))
				{
					UnSubscribe(processedPrimary);
				}
			}

			std::vector<std::tuple<bool, std::shared_ptr<UIObject> > > subscribersCopy;
			subscribersCopy.reserve(m_subscribers.size());
			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (!processedPrimary || (pointerCopy != processedPrimary))
					{
						subscribersCopy.push_back(std::tuple<bool, std::shared_ptr<UIObject> >(std::get<0>(*it), pointerCopy));
					}
				}
			}
			std::unordered_set<UIObjectIdType> toUnSubscribe;
			for (std::tuple<bool, std::shared_ptr<UIObject> > &currentSubscriberTuple : subscribersCopy)
			{
				if (std::get<0>(currentSubscriberTuple))
				{
					if (_NotifyMoveToStrongSubscriber(std::get<1>(currentSubscriberTuple), allTouchState, outProcessedObjects, true))
					{
						toUnSubscribe.insert(std::get<1>(currentSubscriberTuple)->GetId());
					}
				}
				else
				{
					if (_NotifyMoveToWeekSubscriber(std::get<1>(currentSubscriberTuple), allTouchState, outProcessedObjects, true))
					{
						toUnSubscribe.insert(std::get<1>(currentSubscriberTuple)->GetId());
					}
				}
			}

			// Truncate Subscribers
			subscribersCopy.clear();
			m_subscribers.remove_if([&toUnSubscribe](const std::tuple<bool, std::weak_ptr<UIObject> > &value) {
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(value).lock();
				if (!pointerCopy)
				{
					return true;
				}
				return (toUnSubscribe.find(pointerCopy->GetId()) != toUnSubscribe.end());
				});
		}

		//------------------------------------------------------------------------------
		// Notify move event to strong subscribers
		bool UITouch::_NotifyMoveToStrongSubscriber(std::shared_ptr<UIObject> &targetObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects, bool isRealMoved)
		{
			if (!targetObject->IsVisible() || !targetObject->IsEnable())
			{
				std::shared_ptr<UITouchComponent> touchComponent = targetObject->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
				if (touchComponent)
				{
					touchComponent->OnTouchCancel(shared_from_this(), m_currentChangeTime);
				}
				return true;
			}

			std::shared_ptr<UITouchComponent> touchComponent = targetObject->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
			if (touchComponent)
			{
				UIObjectIdType currentObjectId = targetObject->GetId();
				outProcessedObjects.insert(currentObjectId);
				std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >::iterator stateIt = allTouchState.find(currentObjectId);
				if (stateIt == allTouchState.end())
				{
					if (m_lastTouchInObjects.find(currentObjectId) == m_lastTouchInObjects.end())
					{
						if (isRealMoved)
						{
							touchComponent->OnTouchMove(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
						}
					}
					else
					{
						touchComponent->OnTouchOut(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
					}
				}
				else
				{
					if (std::get<1>(stateIt->second))
					{
						if (std::get<2>(stateIt->second))
						{
							if (m_lastTouchInObjects.find(currentObjectId) == m_lastTouchInObjects.end())
							{
								touchComponent->OnTouchIn(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime,
									(m_onceInObjects.find(currentObjectId) == m_onceInObjects.end()),
									true);
							}
							else
							{
								if (isRealMoved)
								{
									touchComponent->OnTouchMove(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
								}
							}
						}
						else
						{
							touchComponent->OnTouchCovered(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
						}
					}
					else
					{
						if (m_lastTouchInObjects.find(currentObjectId) == m_lastTouchInObjects.end())
						{
							touchComponent->OnTouchIn(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime,
								(m_onceInObjects.find(currentObjectId) == m_onceInObjects.end()),
								std::get<2>(stateIt->second));
						}
						else
						{
							if (isRealMoved)
							{
								touchComponent->OnTouchMove(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
							}
						}
					}
				}
			}

			return false;
		}

		//------------------------------------------------------------------------------
		// Notify move event to weak subscribers and return whether to auto unsubcribe
		bool UITouch::_NotifyMoveToWeekSubscriber(std::shared_ptr<UIObject> &targetObject, std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects, bool isRealMoved)
		{
			bool isUnSubscribed = false;
			std::shared_ptr<UITouchComponent> touchComponent = targetObject->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
			if (touchComponent)
			{
				UIObjectIdType currentObjectId = targetObject->GetId();
				outProcessedObjects.insert(currentObjectId);
				std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> >::iterator stateIt = allTouchState.find(currentObjectId);
				if (stateIt == allTouchState.end())
				{
					touchComponent->OnTouchOut(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
					isUnSubscribed = true;
				}
				else
				{
					if (std::get<1>(stateIt->second))
					{
						if (std::get<2>(stateIt->second))
						{
							if (isRealMoved)
							{
								touchComponent->OnTouchMove(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
							}
						}
						else
						{
							touchComponent->OnTouchCovered(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
							isUnSubscribed = true;
						}
					}
					else
					{
						if (isRealMoved)
						{
							touchComponent->OnTouchMove(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
						}
					}
				}
			}

			return isUnSubscribed;
		}

		//------------------------------------------------------------------------------
		void UITouch::_ProcessTouchUpEvent()
		{
			if (m_primarySubscriber)
			{
				std::shared_ptr<UITouchComponent> touchComponent = m_primarySubscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
				if (touchComponent)
				{
					touchComponent->OnTouchUp(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
				}
			}

			std::vector<std::shared_ptr<UIObject> > subscribersCopy;
			subscribersCopy.reserve(m_subscribers.size());
			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (!m_primarySubscriber || (pointerCopy != m_primarySubscriber))
					{
						subscribersCopy.push_back(pointerCopy);
					}
				}
			}
			for (std::shared_ptr<UIObject> &currentSubscriber : subscribersCopy)
			{
				std::shared_ptr<UITouchComponent> touchComponent = currentSubscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
				if (touchComponent)
				{
					touchComponent->OnTouchUp(shared_from_this(), m_currentX, m_currentY, m_currentChangeTime);
				}
			}

			m_primarySubscriber.reset();
			m_subscribers.clear();
		}

		//------------------------------------------------------------------------------
		void UITouch::_ProcessTouchCancelEvent()
		{
			if (m_primarySubscriber)
			{
				std::shared_ptr<UITouchComponent> touchComponent = m_primarySubscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
				if (touchComponent)
				{
					touchComponent->OnTouchCancel(shared_from_this(), m_currentChangeTime);
				}
			}

			std::vector<std::shared_ptr<UIObject> > subscribersCopy;
			subscribersCopy.reserve(m_subscribers.size());
			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (!m_primarySubscriber || (pointerCopy != m_primarySubscriber))
					{
						subscribersCopy.push_back(pointerCopy);
					}
				}
			}
			for (std::shared_ptr<UIObject> &currentSubscriber : subscribersCopy)
			{
				std::shared_ptr<UITouchComponent> touchComponent = currentSubscriber->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
				if (touchComponent)
				{
					touchComponent->OnTouchCancel(shared_from_this(), m_currentChangeTime);
				}
			}

			m_primarySubscriber.reset();
			m_subscribers.clear();
		}

		//------------------------------------------------------------------------------
		void UITouch::_UpdateTouchState(std::unordered_map<UIObjectIdType, std::tuple<std::shared_ptr<UIObject>, bool, bool> > &allTouchState, std::unordered_set<UIObjectIdType> &outProcessedObjects, std::unordered_set<UIObjectIdType> &inTreeObjectIdTable)
		{
			std::shared_ptr<UIObject> processedPrimary(m_primarySubscriber);
			if (processedPrimary)
			{
				if (inTreeObjectIdTable.find(processedPrimary->GetId()) == inTreeObjectIdTable.end())
				{
					std::shared_ptr<UITouchComponent> touchComponent = processedPrimary->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
					if (touchComponent)
					{
						touchComponent->OnTouchCancel(shared_from_this(), m_currentChangeTime);
					}
					UnSubscribe(processedPrimary);
				}
				else if (_NotifyMoveToStrongSubscriber(processedPrimary, allTouchState, outProcessedObjects, false))
				{
					UnSubscribe(processedPrimary);
				}
			}

			std::vector<std::tuple<bool, std::shared_ptr<UIObject> > > subscribersCopy;
			subscribersCopy.reserve(m_subscribers.size());
			for (std::list<std::tuple<bool, std::weak_ptr<UIObject> > >::iterator it = m_subscribers.begin(); it != m_subscribers.end(); ++it)
			{
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(*it).lock();
				if (pointerCopy)
				{
					if (!processedPrimary || (pointerCopy != processedPrimary))
					{
						subscribersCopy.push_back(std::tuple<bool, std::shared_ptr<UIObject> >(std::get<0>(*it), pointerCopy));
					}
				}
			}
			std::unordered_set<UIObjectIdType> toUnSubscribe;
			for (std::tuple<bool, std::shared_ptr<UIObject> > &currentSubscriberTuple : subscribersCopy)
			{
				if (inTreeObjectIdTable.find(std::get<1>(currentSubscriberTuple)->GetId()) == inTreeObjectIdTable.end())
				{
					std::shared_ptr<UITouchComponent> touchComponent = std::get<1>(currentSubscriberTuple)->GetComponentObject<UITouchComponent>(UIComponentType::kTouchInteraction);
					if (touchComponent)
					{
						touchComponent->OnTouchCancel(shared_from_this(), m_currentChangeTime);
					}
					toUnSubscribe.insert(std::get<1>(currentSubscriberTuple)->GetId());
				}
				else
				{
					if (std::get<0>(currentSubscriberTuple))
					{
						if (_NotifyMoveToStrongSubscriber(std::get<1>(currentSubscriberTuple), allTouchState, outProcessedObjects, false))
						{
							toUnSubscribe.insert(std::get<1>(currentSubscriberTuple)->GetId());
						}
					}
					else
					{
						if (_NotifyMoveToWeekSubscriber(std::get<1>(currentSubscriberTuple), allTouchState, outProcessedObjects, false))
						{
							toUnSubscribe.insert(std::get<1>(currentSubscriberTuple)->GetId());
						}
					}
				}
			}

			// Truncate Subscribers
			subscribersCopy.clear();
			m_subscribers.remove_if([&toUnSubscribe](const std::tuple<bool, std::weak_ptr<UIObject> > &value) {
				std::shared_ptr<UIObject> pointerCopy = std::get<1>(value).lock();
				if (!pointerCopy)
				{
					return true;
				}
				return (toUnSubscribe.find(pointerCopy->GetId()) != toUnSubscribe.end());
				});
		}
	}
}
