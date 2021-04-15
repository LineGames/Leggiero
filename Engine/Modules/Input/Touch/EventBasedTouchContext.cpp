////////////////////////////////////////////////////////////////////////////////
// Touch/EventBasedTouchContext.cpp (Leggiero/Modules - Input)
//
// Event based touch context implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "EventBasedTouchContext.h"

// External Library
#include <errno.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Input
#include "../TouchInputComponent.h"


namespace Leggiero
{
	namespace Input
	{
		namespace Touch
		{
			//////////////////////////////////////////////////////////////////////////////// TouchInputComponent

			//------------------------------------------------------------------------------
			EventBasedTouchContext::EventBasedTouchContext(TouchInputComponent *baseInputComponent, size_t queueLengthLimit)
				: m_touchInputComponent(baseInputComponent)
				, m_isSubscribing(false), m_queueLengthLimit(queueLengthLimit)
			{
			}

			//------------------------------------------------------------------------------
			EventBasedTouchContext::~EventBasedTouchContext()
			{
				// Unsubscribe
				if (m_isSubscribing.load())
				{
					m_touchInputComponent->UnRegisterImmediateEventObserver(this);
				}

				// Empty Event Queue
				TouchEvent eventBuffer;
				while (m_queuedEvents.try_dequeue(eventBuffer))
				{
				}
			}

			//------------------------------------------------------------------------------
			void EventBasedTouchContext::StartProcessing()
			{
				int lockResult = pthread_mutex_lock(&m_subscribeLock.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_subscribeLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					if (!m_isSubscribing.load())
					{
						m_touchInputComponent->RegisterImmediateEventObserver(this);
						m_isSubscribing.store(true);
					}
				}
				else
				{
					// Anyway, Go
					bool notSubscribing = false;
					if (m_isSubscribing.compare_exchange_strong(notSubscribing, true))
					{
						m_touchInputComponent->RegisterImmediateEventObserver(this);
					}
				}
			}

			//------------------------------------------------------------------------------
			void EventBasedTouchContext::StopProcessing()
			{
				int lockResult = pthread_mutex_lock(&m_subscribeLock.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_subscribeLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					if (m_isSubscribing.load())
					{
						m_touchInputComponent->UnRegisterImmediateEventObserver(this);
						m_isSubscribing.store(false);
					}
				}
				else
				{
					// Anyway, Go
					if (m_isSubscribing.load())
					{
						m_touchInputComponent->UnRegisterImmediateEventObserver(this);
						m_isSubscribing.store(false);
					}
				}
			}

			//------------------------------------------------------------------------------
			void EventBasedTouchContext::ClearContextState()
			{
				// Clear States
				int lockResult = pthread_rwlock_wrlock(&m_activeTouchMapLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_activeTouchMapLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_activeTouchMap.clear();
				}
				else
				{
					// Anyway, go
					m_activeTouchMap.clear();
				}

				// Empty Event Queue
				TouchEvent eventBuffer;
				while (m_queuedEvents.try_dequeue(eventBuffer))
				{
				}
			}

			//------------------------------------------------------------------------------
			TouchEvent EventBasedTouchContext::DequeueEvent()
			{
				TouchEvent eventBuffer;
				if (m_queuedEvents.try_dequeue(eventBuffer))
				{
					return eventBuffer;
				}

				eventBuffer.MakeInvalid();
				return eventBuffer;
			}

			//------------------------------------------------------------------------------
			bool EventBasedTouchContext::IsTouchActive(TouchIdType touchId)
			{
				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_activeTouchMapLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *mapLock = &m_activeTouchMapLock.GetLock();
					auto releaseLockFunc = [mapLock]() mutable { pthread_rwlock_unlock(mapLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					std::unordered_map<TouchIdType, LastTouchState>::iterator findIt = m_activeTouchMap.find(touchId);
					if (findIt != m_activeTouchMap.end())
					{
						return (findIt->second.IsValid() && findIt->second.isTouchDowned);
					}
				}

				return false;
			}

			//------------------------------------------------------------------------------
			LastTouchState EventBasedTouchContext::GetActiveTouchState(TouchIdType touchId)
			{
				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_activeTouchMapLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *mapLock = &m_activeTouchMapLock.GetLock();
					auto releaseLockFunc = [mapLock]() mutable { pthread_rwlock_unlock(mapLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					std::unordered_map<TouchIdType, LastTouchState>::iterator findIt = m_activeTouchMap.find(touchId);
					if (findIt != m_activeTouchMap.end())
					{
						return findIt->second;
					}
				}

				return LastTouchState();
			}

			//------------------------------------------------------------------------------
			std::vector<LastTouchState> EventBasedTouchContext::GetAllActiveTouchState()
			{
				std::vector<LastTouchState> allTouchStates;
				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_activeTouchMapLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *mapLock = &m_activeTouchMapLock.GetLock();
					auto releaseLockFunc = [mapLock]() mutable { pthread_rwlock_unlock(mapLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					for (std::unordered_map<TouchIdType, LastTouchState>::iterator it = m_activeTouchMap.begin(); it != m_activeTouchMap.end(); ++it)
					{
						allTouchStates.push_back(it->second);
					}
				}

				return allTouchStates;
			}

			//------------------------------------------------------------------------------
			// Handle Touch Down Event
			void EventBasedTouchContext::OnTouchDown(TouchIdType touchId, TouchCoordType x, TouchCoordType y, GameTimeClockType::time_point eventTime)
			{
				TouchEvent eventEntry(TouchEventType::kDown, touchId, eventTime, x, y);

				bool isActiveTouchUpdated = false;
				int lockResult = pthread_rwlock_wrlock(&m_activeTouchMapLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_activeTouchMapLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					std::unordered_map<TouchIdType, LastTouchState>::iterator findIt = m_activeTouchMap.find(touchId);
					if (findIt == m_activeTouchMap.end())
					{
						LastTouchState creatingTouchState(eventEntry);
						m_activeTouchMap.insert(std::make_pair(touchId, creatingTouchState));
					}
					else
					{
						// Something Exceptional Case...
						findIt->second.UpdateTouchState(eventEntry);
					}

					isActiveTouchUpdated = true;
				}
				else
				{
					// Don't get touch without lock
				}

				if (isActiveTouchUpdated)
				{
					m_queuedEvents.enqueue(eventEntry);
					_TrimEventQueue();
				}
			}

			//------------------------------------------------------------------------------
			// Handle Touch Move Event
			void EventBasedTouchContext::OnTouchMoved(TouchIdType touchId, TouchCoordType x, TouchCoordType y, GameTimeClockType::time_point eventTime)
			{
				TouchEvent eventEntry(TouchEventType::kMove, touchId, eventTime, x, y);

				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_activeTouchMapLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}

				bool isActiveTouchUpdated = false;
				if (readLockResult == 0)
				{
					pthread_rwlock_t *mapLock = &m_activeTouchMapLock.GetLock();
					auto releaseLockFunc = [mapLock]() mutable { pthread_rwlock_unlock(mapLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					std::unordered_map<TouchIdType, LastTouchState>::iterator findIt = m_activeTouchMap.find(touchId);
					if (findIt != m_activeTouchMap.end())
					{
						findIt->second.UpdateTouchState(eventEntry);
						isActiveTouchUpdated = true;
					}
				}

				if (isActiveTouchUpdated)
				{
					m_queuedEvents.enqueue(eventEntry);
					_TrimEventQueue();
				}
			}

			//------------------------------------------------------------------------------
			// Handle Touch Up Event
			void EventBasedTouchContext::OnTouchUp(TouchIdType touchId, TouchCoordType x, TouchCoordType y, GameTimeClockType::time_point eventTime)
			{
				TouchEvent eventEntry(TouchEventType::kUp, touchId, eventTime, x, y);

				constexpr int kLockMaximumRetry = 3;
				bool isProcessed = false;
				for (int i = 0; i < kLockMaximumRetry; ++i)
				{
					int lockResult = pthread_rwlock_wrlock(&m_activeTouchMapLock.GetLock());
					if (lockResult == 0)
					{
						pthread_rwlock_t *lockCopy = &m_activeTouchMapLock.GetLock();
						auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						m_activeTouchMap.erase(touchId);

						isProcessed = true;
						break;
					}
				}

				// Fallback Processing
				if (!isProcessed)
				{
					int readLockResult;
					while ((readLockResult = pthread_rwlock_rdlock(&m_activeTouchMapLock.GetLock())) == EAGAIN)
					{
						sched_yield();
					}

					bool isActiveTouchUpdated = false;
					if (readLockResult == 0)
					{
						pthread_rwlock_t *mapLock = &m_activeTouchMapLock.GetLock();
						auto releaseLockFunc = [mapLock]() mutable { pthread_rwlock_unlock(mapLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

						std::unordered_map<TouchIdType, LastTouchState>::iterator findIt = m_activeTouchMap.find(touchId);
						if (findIt != m_activeTouchMap.end())
						{
							findIt->second.UpdateTouchState(eventEntry);
							isActiveTouchUpdated = true;
						}
					}
					
					// If this fails, state update can be missing (but I expected there is almost no those cases)
				}

				m_queuedEvents.enqueue(eventEntry);
				_TrimEventQueue();
			}

			//------------------------------------------------------------------------------
			// Handle Touch Cancel Event by App Interrupt, or ...
			void EventBasedTouchContext::OnTouchCanceled(TouchIdType touchId, GameTimeClockType::time_point eventTime)
			{
				TouchEvent eventEntry(TouchEventType::kCancel, touchId, eventTime);

				constexpr int kLockMaximumRetry = 3;
				bool isProcessed = false;
				for (int i = 0; i < kLockMaximumRetry; ++i)
				{
					int lockResult = pthread_rwlock_wrlock(&m_activeTouchMapLock.GetLock());
					if (lockResult == 0)
					{
						pthread_rwlock_t *lockCopy = &m_activeTouchMapLock.GetLock();
						auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						m_activeTouchMap.erase(touchId);

						isProcessed = true;
						break;
					}
				}

				// Fallback Processing
				if (!isProcessed)
				{
					int readLockResult;
					while ((readLockResult = pthread_rwlock_rdlock(&m_activeTouchMapLock.GetLock())) == EAGAIN)
					{
						sched_yield();
					}

					bool isActiveTouchUpdated = false;
					if (readLockResult == 0)
					{
						pthread_rwlock_t *mapLock = &m_activeTouchMapLock.GetLock();
						auto releaseLockFunc = [mapLock]() mutable { pthread_rwlock_unlock(mapLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

						std::unordered_map<TouchIdType, LastTouchState>::iterator findIt = m_activeTouchMap.find(touchId);
						if (findIt != m_activeTouchMap.end())
						{
							findIt->second.UpdateTouchState(eventEntry);
							isActiveTouchUpdated = true;
						}
					}

					// If this fails, state update can be missing (but I expected there is almost no those cases)
				}

				m_queuedEvents.enqueue(eventEntry);
				_TrimEventQueue();
			}

			//------------------------------------------------------------------------------
			void EventBasedTouchContext::_TrimEventQueue()
			{
				size_t queueLengthLimitCopy = m_queueLengthLimit;
				if (queueLengthLimitCopy <= 0)
				{
					return;
				}

				constexpr int kMaxFailRetryCount = 3;
				int dequeueFailCount = 0;

				TouchEvent eventBuffer;
				while (m_queuedEvents.size_approx() > queueLengthLimitCopy)
				{
					if (!m_queuedEvents.try_dequeue(eventBuffer))
					{
						++dequeueFailCount;
						if (dequeueFailCount > kMaxFailRetryCount)
						{
							break;
						}
					}
					else
					{
						dequeueFailCount = 0;
					}
				}
			}
		}
	}
}
