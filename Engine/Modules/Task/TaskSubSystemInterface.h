////////////////////////////////////////////////////////////////////////////////
// TaskSubSystemInterface.h (Leggiero/Modules - Task)
//
// Interface Definition for Task Sub-Systems
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASK_SUB_SYSTEM_INTERFACE_H
#define __LM_TASK__TASK_SUB_SYSTEM_INTERFACE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <set>

// Leggiero.Engine
#include <Engine/Module/EngineModuleId.h>
#include <Engine/Module/EngineComponentId.h>

// Leggiero.Task
#include "TaskTypes.h"


namespace Leggiero
{
	// Forward Declarations
	class EngineComponent;

	namespace Engine
	{
		class GameProcessAnchor;
	}

	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Task
	{
		// Forward Declarations
		class TaskManagerComponent;
		class ITaskProcessor;


		// Base Interface for Task Sub-System
		class ITaskSubSystem
		{
		public:
			virtual ~ITaskSubSystem() { }

		public:
			virtual void CollectModuleDependency(std::set<EngineModuleIdType> dependencySet) const { }
			virtual void CollectComponentDependency(std::set<EngineComponentIdType> dependencySet) const { }
			virtual void InjectComponentDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) { }

			virtual void InitializeSubSystem(TaskManagerComponent *ownerManager,
				Application::ApplicationComponent *application, Engine::GameProcessAnchor *gameAnchor) { }
			virtual void ShutdownSubSystem() { }

			virtual void CreateProcessors(std::function<void(std::shared_ptr<ITaskProcessor>)> registerFunc) { }
		};
	}
}

#endif
