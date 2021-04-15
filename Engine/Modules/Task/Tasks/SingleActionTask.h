////////////////////////////////////////////////////////////////////////////////
// Tasks/SingleActionTask.h (Leggiero/Modules - Task)
//
// Task executes a simple task
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASKS__SINGLE_ACTION_TASK_H
#define __LM_TASK__TASKS__SINGLE_ACTION_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>

// Leggiero.Task
#include "ITask.h"


namespace Leggiero
{
	namespace Task
	{
		// Task that executes a given action once
		class SingleActionTask
			: public ITask
		{
		public:
			// Type of executable Action
			using ActionFuncType = std::function<void()>;

		public:
			SingleActionTask(ActionFuncType action, 
				TaskCapabilityType capabilities = TaskCapabilities::kGeneral, TaskPriorityClass priority = TaskPriorityClass::kDefault, TaskPropertyType taskProperty = TaskPropertyType::kNone);
			virtual ~SingleActionTask() { }

		public:	// Task Properties
			virtual TaskPriorityClass	GetTaskPriority() override { return m_taskPriority; }
			virtual TaskPropertyType	GetTaskProperty() override { return m_taskProperty; }
			virtual TaskCapabilityType	GetRequiredCapabilties() override { return m_taskCapabilities; }

		public:
			// Do Real Task Works
			virtual TaskDoneResult Do() override;

		protected:
			ActionFuncType m_action;

			TaskPriorityClass	m_taskPriority;
			TaskPropertyType	m_taskProperty;
			TaskCapabilityType	m_taskCapabilities;
		};
	}
}

#endif
