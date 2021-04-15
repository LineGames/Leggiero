////////////////////////////////////////////////////////////////////////////////
// _Internal/ITaskManagerSystemFunctions.h (Leggiero/Modules - Task)
//
// Task Manager Interface for Task Systems
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK___INTERNAL__I_TASK_MANAGER_SYSTEM_FUNCTIONS_H
#define __LM_TASK___INTERNAL__I_TASK_MANAGER_SYSTEM_FUNCTIONS_H


// Leggiero.Task
#include "../TaskTypes.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declarations
		struct TaskExecutionEntry;
		class ITaskSubSystem;


		// Interface for Task Systems
		class ITaskManagerSystemFunctions
		{
		public:
			virtual ~ITaskManagerSystemFunctions() { }

		public:	// For sub-systems
			virtual void AttachSubSystem(std::shared_ptr<ITaskSubSystem> subSystem) = 0;
			virtual void AddGeneralWorkerCapability(TaskCapabilityType capabilities) = 0;

		public:	// For processors
			virtual void RequestExecution(TaskExecutionEntry *execution) = 0;
			virtual void RequestDelayedExecution(TaskExecutionEntry *execution, SchedulingClock::duration delay) = 0;

			virtual void ReleaseExecution(TaskExecutionEntry *execution) = 0;
		};
	}
}

#endif
