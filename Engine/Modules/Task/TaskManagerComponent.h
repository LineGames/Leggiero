////////////////////////////////////////////////////////////////////////////////
// TaskManagerComponent.h (Leggiero/Modules - Task)
//
// Task Manager Engine Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASK_MANAGER_COMPONENT_H
#define __LM_TASK__TASK_MANAGER_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.Task
#include "TaskTypes.h"
#include "_Internal/ITaskManagerSystemFunctions.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declarations
		class ITask;
		class ITaskProcessor;


		// Task Manager Component
		class TaskManagerComponent
			: public EngineComponent
			, public ITaskManagerSystemFunctions
		{
		public:	// Can be created by Creation Function
			static TaskManagerComponent *CreateComponentObject();

		public:
			TaskManagerComponent();
			virtual ~TaskManagerComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kTaskManager; };

		public:
			virtual bool ExecuteTask(std::shared_ptr<ITask> task) = 0;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Task::TaskManagerComponent, Leggiero::EngineComponentIdType::kTaskManager);

#endif
