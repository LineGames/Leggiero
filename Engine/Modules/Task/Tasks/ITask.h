////////////////////////////////////////////////////////////////////////////////
// Tasks/ITask.h (Leggiero/Modules - Task)
//
// Task Base Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASKS__I_TASK_H
#define __LM_TASK__TASKS__I_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>

// Leggiero.Task
#include "../TaskTypes.h"


namespace Leggiero
{
	namespace Task
	{
		// Interface for a Task
		class ITask
		{
		public:
			ITask()
				: m_currentState(TaskState::kNone)
			{ }

			virtual ~ITask() { }

		public:	// Task Properties
			virtual TaskPriorityClass	GetTaskPriority() { return TaskPriorityClass::kDefault; }
			virtual TaskPropertyType	GetTaskProperty() { return TaskPropertyType::kNone; }
			virtual TaskCapabilityType	GetRequiredCapabilties() { return TaskCapabilities::kGeneral; }

		public:	// Task States
			// Get Current State of the Task
			TaskState GetTaskState() const { return m_currentState.load(std::memory_order_consume); }
			std::atomic<TaskState> &State() { return m_currentState; }

			// Check whether the task is ready or not
			virtual bool IsTaskReady() { return true; }

			// Check Task Execution Done
			bool IsFinished() const { return Utility::SyntacticSugar::HasFlag(GetTaskState(), TaskState::kJobFinished); }
			// Check there was an error during task execution
			bool HasError() const { return Utility::SyntacticSugar::HasFlag(GetTaskState(), TaskState::kHasError); }

		public:	// Task Implementations
			// Do Real Task Works
			virtual TaskDoneResult Do() = 0;

			// Pre-Process Function before Task All Processing
			virtual void OnBeforeProcess() { }
			// Pre-Process Function before Each Task All Processing Step
			virtual void OnBeforeStepProcess() { }
			// Function called after Each Task All Processing Step
			virtual void OnAfterStepProcess() { }
			// Function called after Task All Processing
			virtual void OnAfterProcess() { }

		public:	// for task system
			// Set Error Flag on Task State
			virtual void SetErrorFlag()
			{
				TaskState storedValue;
				TaskState flagedValue;
				do
				{
					storedValue = m_currentState.load();
					flagedValue = (storedValue | TaskState::kHasError);
				} while (!m_currentState.compare_exchange_weak(storedValue, flagedValue));
			}

		protected:
			std::atomic<TaskState>	m_currentState;
		};
	}
}

#endif
