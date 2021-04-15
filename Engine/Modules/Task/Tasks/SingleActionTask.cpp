////////////////////////////////////////////////////////////////////////////////
// Tasks/SingleActionTask.cpp (Leggiero/Modules - Task)
//
// Single Action Task Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "SingleActionTask.h"


namespace Leggiero
{
	namespace Task
	{
		//////////////////////////////////////////////////////////////////////////////// SingleActionTask

		//------------------------------------------------------------------------------
		SingleActionTask::SingleActionTask(ActionFuncType action, TaskCapabilityType capabilities, TaskPriorityClass priority, TaskPropertyType taskProperty)
			: m_action(action)
			, m_taskPriority(priority), m_taskProperty(taskProperty), m_taskCapabilities(capabilities)
		{
		}

		//------------------------------------------------------------------------------
		TaskDoneResult SingleActionTask::Do()
		{
			if (m_action)
			{
				m_action();
			}
			return TaskDoneResult(TaskDoneResult::ResultType::kFinished);
		}
	}
}
