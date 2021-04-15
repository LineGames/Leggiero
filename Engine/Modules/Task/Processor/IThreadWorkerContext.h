////////////////////////////////////////////////////////////////////////////////
// Processor/IThreadWorkerContext.h (Leggiero/Modules - Task)
//
// Thread Worker Processing Context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__PROCESSOR__I_THREAD_WORKER_CONTEXT_H
#define __LM_TASK__PROCESSOR__I_THREAD_WORKER_CONTEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <pthread.h>

// Leggiero.Task
#include "../TaskTypes.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declaration
		struct TaskExecutionEntry;


		// Thread Worker Context
		class IThreadWorkerContext
		{
		public:
			virtual ~IThreadWorkerContext() { }

		public:
			virtual pthread_mutex_t &QueueMutex() = 0;
			virtual pthread_cond_t &QueueCondition() = 0;

			virtual pthread_mutex_t &PauseMutex() = 0;
			virtual pthread_cond_t &PauseCondition() = 0;

			virtual void HandleDone(TaskExecutionEntry *execution) = 0;
			virtual void HandleYield(TaskExecutionEntry *execution) = 0;
			virtual void HandleWait(TaskExecutionEntry *execution, TaskDoneResult waitResult) = 0;
			virtual void HandleError(TaskExecutionEntry *execution) = 0;

			virtual void ReleaseNotExecution(TaskExecutionEntry *execution) = 0;

			virtual TaskExecutionEntry *DequeueJob() = 0;
			virtual bool IsQueueEmptyRoughly() = 0;
		};
	}
}

#endif
