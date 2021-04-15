////////////////////////////////////////////////////////////////////////////////
// Processor/ITaskProcessor.h (Leggiero/Modules - Task)
//
// Interface for Task Processor Units
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__PROCESSOR__I_TASK_PROCESSOR_H
#define __LM_TASK__PROCESSOR__I_TASK_PROCESSOR_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Task
#include "../TaskTypes.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declaration
		struct TaskExecutionEntry;


		// Interface of Task Processor Unit
		class ITaskProcessor
		{
		public:
			virtual ~ITaskProcessor() { }

		public:
			// Give job to processor
			virtual void GiveJob(TaskExecutionEntry *job) = 0;

			// Get task capability of the processor
			virtual TaskCapabilityType GetProcessorTaskCapability() { return TaskCapabilities::kGeneral; }

			// Is processor handle sleeps of its tasks?
			virtual bool IsManagingSleeps() { return false; }

		public:
			virtual void PrepareProcessorShutdown() { }
		};
	}
}

#endif
