////////////////////////////////////////////////////////////////////////////////
// Processor/ThreadWorker.h (Leggiero/Modules - Task)
//
// Task process worker based on a thread
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__PROCESSOR__THREAD_WORKER_H
#define __LM_TASK__PROCESSOR__THREAD_WORKER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>

// External Library
#include <pthread.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Task
#include "../TaskTypes.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declarations
		class ITask;
		class IThreadWorkerContext;

		struct TaskExecutionEntry;

		namespace _Internal
		{
			struct LoopProcessContext;
		}


		// Thread Worker
		class ThreadWorker
		{
		public:
			ThreadWorker(IThreadWorkerContext *pOwnerContext);
			virtual ~ThreadWorker();

		public:
			bool IsRunning();
			void RequestStop();

			bool IsPaused();
			void RequestPause();
			void RequestResume();

		protected:	// To modify behavior by sub-classes
			virtual bool _InitializeBeforeWork() { return true; }
			virtual void _FinalizeAfterWork() { }

			virtual void _PreProcessBeforeTask() { }
			virtual void _PostProcessAfterTask() { }

			virtual bool _LoopProcess() { return true; }

			virtual long _CalculateLoopDelay(_Internal::LoopProcessContext &loopContext);

		protected:
			void _ThreadFunction();

			void _ProcessTask(TaskExecutionEntry *execution, _Internal::LoopProcessContext &loopContext);

			void _WaitLoop(_Internal::LoopProcessContext &loopContext);
			void _WaitPause();

		protected:
			static void *_ThreadStartHelper(void *threadThis);

		protected:
			IThreadWorkerContext *m_pOwnerContext;

			std::atomic_bool	m_isShutdownRequested;
			std::atomic_bool	m_isThreadFinished;
			pthread_t			m_workerThread;

			std::atomic_bool	m_isPauseRequested;
			std::atomic_bool	m_isThreadPaused;
		};
	}
}

#endif
