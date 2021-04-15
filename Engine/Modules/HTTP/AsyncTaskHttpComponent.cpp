////////////////////////////////////////////////////////////////////////////////
// AsyncTaskHttpComponent.cpp (Leggiero/Modules - HTTP)
//
// Task-based Async HTTP Processor Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsyncTaskHttpComponent.h"

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Task
#ifndef _LEGGIERO_IOS
#include <Task/TaskManagerComponent.h>
#endif


DEFINE_GET_COMPONENT_INTERFACE(Leggiero::HTTP::Async::AsyncTaskHttpComponent, Leggiero::EngineComponentIdType::kAsyncTaskHTTP);


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			//////////////////////////////////////////////////////////////////////////////// AsyncTaskHttpComponent

			//------------------------------------------------------------------------------
			AsyncTaskHttpComponent::AsyncTaskHttpComponent()
			{

			}

			//------------------------------------------------------------------------------
			AsyncTaskHttpComponent::~AsyncTaskHttpComponent()
			{

			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void AsyncTaskHttpComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void AsyncTaskHttpComponent::ShutdownComponent()
			{
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent modules needed by this component
			const std::vector<EngineModuleIdType> &AsyncTaskHttpComponent::GetDependentModules() const
			{
				static std::vector<EngineModuleIdType> dependentModuleList{
					EngineModuleIdType::kHTTP
				};
				return dependentModuleList;
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent components needed by this component
			const std::vector<EngineComponentIdType> &AsyncTaskHttpComponent::GetDependentComponents() const
			{
#ifdef _LEGGIERO_IOS
				static std::vector<EngineComponentIdType> dependentComponentList{ };
#else
				static std::vector<EngineComponentIdType> dependentComponentList{
					EngineComponentIdType::kTaskManager
				};
#endif
				return dependentComponentList;
			}

			//------------------------------------------------------------------------------
			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			void AsyncTaskHttpComponent::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
			{
#ifndef _LEGGIERO_IOS
				switch (componentId)
				{
					case EngineComponentIdType::kTaskManager:
						{
							m_taskManager = dynamic_cast<Task::TaskManagerComponent *>(dependentComponent);
						}
						break;
				}
#endif
			}
		}
	}
}
