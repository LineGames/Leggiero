////////////////////////////////////////////////////////////////////////////////
// Tasks/DependentTask.cpp (Leggiero/Modules - Task)
//
// Dependent Task Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "DependentTask.h"


namespace Leggiero
{
	namespace Task
	{
		//////////////////////////////////////////////////////////////////////////////// DependentTask

		//------------------------------------------------------------------------------
		DependentTask::DependentTask(std::shared_ptr<ITask> priorTask)
		{
			m_dependsOn.push_back(priorTask);
		}

		//------------------------------------------------------------------------------
		DependentTask::DependentTask(std::initializer_list<std::shared_ptr<ITask> > priorTasks)
			: m_dependsOn(priorTasks)
		{
		}

		//------------------------------------------------------------------------------
		DependentTask::DependentTask(const std::vector<std::shared_ptr<ITask> > &priorTasks)
			: m_dependsOn(priorTasks)
		{
		}

		//------------------------------------------------------------------------------
		DependentTask::DependentTask(std::vector<std::shared_ptr<ITask> > &&priorTasks)
			: m_dependsOn(priorTasks)
		{
		}

		//------------------------------------------------------------------------------
		// Check whether the task is ready or not
		bool DependentTask::IsTaskReady()
		{
			for (const std::shared_ptr<ITask> &checkingTask : m_dependsOn)
			{
				if (!checkingTask)
				{
					continue;
				}
				if (!checkingTask->IsFinished())
				{
					return false;
				}
			}
			return true;
		}

		//------------------------------------------------------------------------------
		// Do Real Task Works
		TaskDoneResult DependentTask::Do()
		{
			if (!IsTaskReady())
			{
				return TaskDoneResult(TaskDoneResult::ResultType::kWaitCondition);
			}

			return _DoWork();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<ITask> DependentTask::GetPriorTaskOf(int index)
		{
			if (index < 0)
			{
				return nullptr;
			}
			if (index >= m_dependsOn.size())
			{
				return nullptr;
			}
			return m_dependsOn[index];
		}
	}
}
