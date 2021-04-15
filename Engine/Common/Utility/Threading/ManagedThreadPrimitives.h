////////////////////////////////////////////////////////////////////////////////
// Threading/ManagedThreadPrimitives.h (Leggiero - Utility)
//
// Life-cycle managed thread primitives as class
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__THREADING__MANAGED_THREAD_PRIMITIVES_H
#define __UTILITY__THREADING__MANAGED_THREAD_PRIMITIVES_H


// Standard Library
#include <memory>

// External Library
#include <pthread.h>

// Leggiero.Utility
#include "../Sugar/NonCopyable.h"


namespace Leggiero
{
	namespace Utility
	{
		namespace Threading
		{
			// Managed pthread mutex based lock
			class SafePthreadLock
			{
			public:
				SafePthreadLock();
				SafePthreadLock(bool isRecursive, bool isProcessShared);
				~SafePthreadLock();

			public:
				pthread_mutex_t &GetLock() { return m_lock; }
				const pthread_mutex_t &GetConstLock() const { return m_lock; }

			public:
				// Auto-Unlocking Context
				// Not threadsafe.
				// Should be destroyed before the end of lifecycle of SafePthreadLock.
				class LockContext
					: public SyntacticSugar::NonCopyable
				{
				protected:
					LockContext() { }

				public:
					virtual ~LockContext() { }

				public:
					virtual void UnlockNow() = 0;
				};

				// Lock and return auto-unlocking handle
				std::unique_ptr<LockContext> Lock();

			public:
				pthread_mutex_t m_lock;
			};


			// Managed pthread RW Lock
			class SafePthreadRWLock
			{
			public:
				SafePthreadRWLock();
				SafePthreadRWLock(bool isProcessShared);
				~SafePthreadRWLock();

			public:
				pthread_rwlock_t &GetLock() { return m_rwLock; }
				const pthread_rwlock_t &GetConstLock() const { return m_rwLock; }

			public:
				pthread_rwlock_t m_rwLock;
			};


			// Managed pthread condition variable
			class SafePthreadConditionVariable
			{
			public:
				SafePthreadConditionVariable();
				SafePthreadConditionVariable(bool isProcessShared);
				~SafePthreadConditionVariable();

			public:
				pthread_cond_t &GetConditionVariable() { return m_conditionVariable; }
				const pthread_cond_t &GetConstConditionVariable() const { return m_conditionVariable; }

			public:
				pthread_cond_t m_conditionVariable;
			};
		}
	}
}

#endif
