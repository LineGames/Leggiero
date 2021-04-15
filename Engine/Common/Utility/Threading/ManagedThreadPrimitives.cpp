////////////////////////////////////////////////////////////////////////////////
// Threading/ManagedThreadPrimitives.cpp (Leggiero - Utility)
//
// Implementation of managed thread primitives
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ManagedThreadPrimitives.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Threading
		{
			//////////////////////////////////////////////////////////////////////////////// SafePthreadLock

			//------------------------------------------------------------------------------
			namespace _Internal
			{
				// Implementation of Concrete Unlocker of SafePthreadLock
				class SafePthreadLockConcreteUnlockContext : public SafePthreadLock::LockContext
				{
				public:
					SafePthreadLockConcreteUnlockContext(SafePthreadLock *safeLock)
						: m_safeLock(safeLock)
					{ }

					virtual ~SafePthreadLockConcreteUnlockContext()
					{
						if (m_safeLock != nullptr)
						{
							pthread_mutex_unlock(&(m_safeLock->GetLock()));
							m_safeLock = nullptr;
						}
					}

					virtual void UnlockNow() override
					{
						if (m_safeLock != nullptr)
						{
							pthread_mutex_unlock(&(m_safeLock->GetLock()));
							m_safeLock = nullptr;
						}
					}

				private:
					SafePthreadLock *m_safeLock;
				};
			}

			//------------------------------------------------------------------------------
			SafePthreadLock::SafePthreadLock()
			{
				pthread_mutex_init(&m_lock, NULL);
			}

			//------------------------------------------------------------------------------
			SafePthreadLock::SafePthreadLock(bool isRecursive, bool isProcessShared)
			{
				pthread_mutexattr_t attr;
				pthread_mutexattr_init(&attr);

				pthread_mutexattr_settype(&attr, isRecursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL);
				pthread_mutexattr_setpshared(&attr, isProcessShared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);

				// Use robust mutex if supported
#ifdef PTHREAD_MUTEX_ROBUST
				pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
#endif

				pthread_mutex_init(&m_lock, &attr);

				pthread_mutexattr_destroy(&attr);
			}

			//------------------------------------------------------------------------------
			SafePthreadLock::~SafePthreadLock()
			{
				pthread_mutex_destroy(&m_lock);
			}

			//------------------------------------------------------------------------------
			// Lock and return auto-unlocking handle
			std::unique_ptr<SafePthreadLock::LockContext> SafePthreadLock::Lock()
			{
				int lockResult = pthread_mutex_lock(&m_lock);
				if (lockResult == 0)
				{
					return std::make_unique< _Internal::SafePthreadLockConcreteUnlockContext>(this);
				}

				// Lock Failed
				return nullptr;
			}


			//////////////////////////////////////////////////////////////////////////////// SafePthreadRWLock

			//------------------------------------------------------------------------------
			SafePthreadRWLock::SafePthreadRWLock()
			{
				pthread_rwlock_init(&m_rwLock, NULL);
			}

			//------------------------------------------------------------------------------
			SafePthreadRWLock::SafePthreadRWLock(bool isProcessShared)
			{
				pthread_rwlockattr_t attr;
				pthread_rwlockattr_init(&attr);

				pthread_rwlockattr_setpshared(&attr, isProcessShared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);

				pthread_rwlock_init(&m_rwLock, &attr);

				pthread_rwlockattr_destroy(&attr);
			}

			//------------------------------------------------------------------------------
			SafePthreadRWLock::~SafePthreadRWLock()
			{
				pthread_rwlock_destroy(&m_rwLock);
			}


			//////////////////////////////////////////////////////////////////////////////// SafePthreadConditionVariable

			//------------------------------------------------------------------------------
			SafePthreadConditionVariable::SafePthreadConditionVariable()
			{
				pthread_cond_init(&m_conditionVariable, NULL);
			}

			//------------------------------------------------------------------------------
			SafePthreadConditionVariable::SafePthreadConditionVariable(bool isProcessShared)
			{
				pthread_condattr_t attr;
				pthread_condattr_init(&attr);

				pthread_condattr_setpshared(&attr, isProcessShared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);

				pthread_cond_init(&m_conditionVariable, &attr);

				pthread_condattr_destroy(&attr);
			}

			//------------------------------------------------------------------------------
			SafePthreadConditionVariable::~SafePthreadConditionVariable()
			{
				pthread_cond_destroy(&m_conditionVariable);
			}
		}
	}
}
