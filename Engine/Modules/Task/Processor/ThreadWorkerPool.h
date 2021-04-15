////////////////////////////////////////////////////////////////////////////////
// Processor/ThreadWorkerPool.h (Leggiero/Modules - Task)
//
// Processor with Pooled Thread Workers
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__PROCESSOR__THREAD_WORKER_POOL_H
#define __LM_TASK__PROCESSOR__THREAD_WORKER_POOL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <list>

// External Library
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Task
#include "../TaskTypes.h"
#include "ITaskProcessor.h"
#include "IThreadWorkerContext.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Task
	{
		// Forward Declarations
		class ITaskManagerSystemFunctions;
		struct TaskExecutionEntry;
		class ThreadWorker;


		// Processor with Thread Worker Pool
		class ThreadWorkerPool
			: public ITaskProcessor
			, public IThreadWorkerContext
			, public Application::IApplicationEventObserver
		{
		public:
			ThreadWorkerPool(ITaskManagerSystemFunctions *pManager, Application::ApplicationComponent *appComponent);
			virtual ~ThreadWorkerPool();

		public:
			int GetWorkerCount();
			int IncreaseWorker(int increaseCount = 1);
			void DecreaseWorker();

			size_t GetApproxQueueSize() { return m_jobs.size_approx(); }

		public:	// ITaskProcessor
			// Give job to processor
			virtual void GiveJob(TaskExecutionEntry *job) override;

			// Get task capability of the processor
			virtual TaskCapabilityType GetProcessorTaskCapability() override { return TaskCapabilities::kGeneral; }

			// Is processor handle sleeps of its tasks?
			virtual bool IsManagingSleeps() override { return false; }

		public:	// IThreadWorkerContext
			virtual pthread_mutex_t &QueueMutex() override { return m_queueMutex.GetLock(); }
			virtual pthread_cond_t &QueueCondition() override { return m_queueCondition.GetConditionVariable(); }

			virtual pthread_mutex_t &PauseMutex() override { return m_pauseMutex.GetLock(); }
			virtual pthread_cond_t &PauseCondition() override { return m_pauseCondition.GetConditionVariable(); }

			virtual void HandleDone(TaskExecutionEntry *execution) override;
			virtual void HandleYield(TaskExecutionEntry *execution) override;
			virtual void HandleWait(TaskExecutionEntry *execution, TaskDoneResult waitResult) override;
			virtual void HandleError(TaskExecutionEntry *execution) override;

			virtual void ReleaseNotExecution(TaskExecutionEntry *execution) override;

			virtual TaskExecutionEntry *DequeueJob() override;
			virtual bool IsQueueEmptyRoughly() override { return m_jobs.size_approx() == 0; }

		public:	// Application::IApplicationEventObserver
			virtual void OnGoToBackground() override;
			virtual void OnReturnFromBackground() override;
			
		protected:	// Behavior Interface
			virtual ThreadWorker *_CreateWorker();

		protected:
			void _CollectInvalidWorkers();

		protected:
			ITaskManagerSystemFunctions *m_pManager;
			Application::ApplicationComponent *m_appComponentCopy;

			Utility::Threading::SafePthreadLock m_queueMutex;
			Utility::Threading::SafePthreadConditionVariable m_queueCondition;

			Utility::Threading::SafePthreadLock m_pauseMutex;
			Utility::Threading::SafePthreadConditionVariable m_pauseCondition;

			Utility::Threading::SafePthreadLock m_workerListLock;
			std::list<ThreadWorker *>			m_workerList;

			moodycamel::ConcurrentQueue<TaskExecutionEntry *> m_jobs;
		};
	}
}

#endif
