////////////////////////////////////////////////////////////////////////////////
// Object/PointerHolder.cpp (Leggiero - Utility)
//
// Pointer Holder Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "PointerHolder.h"

// External Library
#include <errno.h>

// Leggiero.Utility
#include "../Sugar/Finally.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Object
		{
			//////////////////////////////////////////////////////////////////////////////// Hold Context Implementation

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class PointerHoldingContext : public PointerHolder::IHoldContext
				{
				public:
					PointerHoldingContext(pthread_rwlock_t *lock, void *ptr)
						: m_holdingLock(lock), m_holdingPtr(ptr)
					{ }

					virtual ~PointerHoldingContext()
					{
						pthread_rwlock_unlock(m_holdingLock);
					}

					virtual void *GetObject() override
					{
						return m_holdingPtr;
					}

				private:
					pthread_rwlock_t *m_holdingLock;
					void *m_holdingPtr;
				};
			}


			//////////////////////////////////////////////////////////////////////////////// PointerHolder

			//------------------------------------------------------------------------------
			PointerHolder::PointerHolder(void *targetPtr)
				: m_targetPtr(targetPtr)
			{

			}

			//------------------------------------------------------------------------------
			void PointerHolder::NotifyTargetDeleted()
			{
				int tryCount = 0;
				while (true)
				{
					int writeLockResult = pthread_rwlock_wrlock(&m_targetLock.GetLock());
					if (writeLockResult == 0)
					{
						pthread_rwlock_t *targetLock = &m_targetLock.GetLock();
						auto releaseLockFunc = [targetLock]() mutable { pthread_rwlock_unlock(targetLock); };
						FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

						m_targetPtr = nullptr;
						break;
					}

					++tryCount;
					if (tryCount > 3)
					{
						// Go anyway...
						m_targetPtr = nullptr;
						break;
					}

					sched_yield();
				}
			}

			//------------------------------------------------------------------------------
			std::unique_ptr<PointerHolder::IHoldContext> PointerHolder::LockTarget()
			{
				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_targetLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}

				return std::make_unique<_Internal::PointerHoldingContext>(&m_targetLock.GetLock(), m_targetPtr);
			}
		}
	}
}
