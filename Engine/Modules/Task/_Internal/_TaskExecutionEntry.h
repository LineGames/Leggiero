////////////////////////////////////////////////////////////////////////////////
// _Internal/_TaskExecutionEntry.h (Leggiero/Modules - Task)
//
// Task execution entry with processing metadata for task library
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK___INTERNAL___TASK_EXECUTION_ENTRY_H
#define __LM_TASK___INTERNAL___TASK_EXECUTION_ENTRY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Task
#include "../TaskTypes.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declaration
		class ITask;


		// Task Execution Entry
		struct TaskExecutionEntry
		{
		public:
			std::shared_ptr<ITask>		task;
			SchedulingClock::time_point	lastStep;

		public:
			void Clear()
			{
				task.reset();
			}
		};
	}
}

#endif
