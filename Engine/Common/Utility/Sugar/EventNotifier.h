////////////////////////////////////////////////////////////////////////////////
// Sugar/EventNotifier.h (Leggiero - Utility)
//
// Event Notifier System: a variant of Observer Pattern
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__SUGAR__EVENT_NOTIFIER_H
#define __UTILITY__SUGAR__EVENT_NOTIFIER_H


// Standard Library
#include <cstdlib>
#include <algorithm>
#include <list>

// External Library
#include <errno.h>
#include <pthread.h>

// Leggiero.Utility
#include "Finally.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace DesignPattern
		{
			// Event Notifier
			template <typename ObserverT>
			class EventNotifier
			{
			public:
				EventNotifier()
				{
					pthread_rwlock_init(&m_observerListLock, NULL);
				}

				virtual ~EventNotifier()
				{
					pthread_rwlock_destroy(&m_observerListLock);
				}

			public:
				// Notify event for all observers: call notify function for all observers
				template<class UnaryFunction>
				void NotifyEvent(UnaryFunction notifyFunc)
				{
					int readLockResult;
					while ((readLockResult = pthread_rwlock_rdlock(&m_observerListLock)) == EAGAIN)
					{
						sched_yield();
					}
					if (readLockResult == 0)
					{
						pthread_rwlock_t *listLock = &m_observerListLock;
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

						std::for_each(m_observers.begin(), m_observers.end(), notifyFunc);
					}
				}

				// Register an Observer
				void RegisterObserver(ObserverT observer)
				{
					int writeLockResult = pthread_rwlock_wrlock(&m_observerListLock);
					if (writeLockResult == 0)
					{
						pthread_rwlock_t *listLock = &m_observerListLock;
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

						m_observers.push_back(observer);
					}
				}

				// Remove an Observer
				void UnRegisterObserver(ObserverT observer)
				{
					int writeLockResult = pthread_rwlock_wrlock(&m_observerListLock);
					if (writeLockResult == 0)
					{
						pthread_rwlock_t *listLock = &m_observerListLock;
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

						m_observers.remove(observer);
					}
				}

				// Clear ALL Observers
				void ClearObserver()
				{
					int writeLockResult = pthread_rwlock_wrlock(&m_observerListLock);
					if (writeLockResult == 0)
					{
						pthread_rwlock_t *listLock = &m_observerListLock;
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

						m_observers.clear();
					}
				}

			private:
				std::list<ObserverT>	m_observers;
				pthread_rwlock_t		m_observerListLock;
			};
		}
	}
}

#endif
