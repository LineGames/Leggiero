////////////////////////////////////////////////////////////////////////////////
// Tasks/DependentTask.h (Leggiero/Modules - Task)
//
// Task depends on other tasks
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASKS__DEPENDENT_TASK_H
#define __LM_TASK__TASKS__DEPENDENT_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <iterator>
#include <memory>
#include <vector>
#include <utility>

// Leggiero.Task
#include "ITask.h"


namespace Leggiero
{
	namespace Task
	{
		// Task Dependens on Other Tasks
		class DependentTask
			: public ITask
		{
		public:
			DependentTask(std::shared_ptr<ITask> priorTask);
			DependentTask(std::initializer_list<std::shared_ptr<ITask> > priorTasks);
			DependentTask(const std::vector<std::shared_ptr<ITask> > &priorTasks);
			DependentTask(std::vector<std::shared_ptr<ITask> > &&priorTasks);

			template <typename InputIterator>
			DependentTask(InputIterator first, InputIterator last)
				: m_dependsOn(first, last)
			{
			}

			virtual ~DependentTask() { }

		public:	// ITask
			// Check whether the task is ready or not
			virtual bool IsTaskReady() override;

			// Do Real Task Works
			virtual TaskDoneResult Do() override;

		public:
			const std::vector<std::shared_ptr<ITask> > &GetPriorTasks() { return m_dependsOn; }
			std::shared_ptr<ITask> GetPriorTaskOf(int index);

		protected:	// To Override
			virtual TaskDoneResult _DoWork() = 0;

		protected:
			std::vector<std::shared_ptr<ITask> > m_dependsOn;
		};
	}
}

#endif
