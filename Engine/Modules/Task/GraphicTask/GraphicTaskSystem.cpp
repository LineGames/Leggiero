////////////////////////////////////////////////////////////////////////////////
// GraphicTask/GraphicTaskSystem.cpp (Leggiero/Modules - Task)
//
// Graphic Task System Common Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GraphicTaskSystem.h"

// Leggiero.Engine
#include <Engine/Application/GameProcessAnchor.h>
#include <Engine/Application/GameProcessAnchorObserver.h>

// Leggiero.Task
#include "../TaskManagerComponent.h"
#include "../TaskSubSystemInterface.h"
#include "GraphicThreadWorkerPool.h"


namespace Leggiero
{
	namespace Task
	{
		namespace GraphicTask
		{
			//////////////////////////////////////////////////////////////////////////////// TaskManagerComponent

			//------------------------------------------------------------------------------
			// Subsystem for Graphic Tasks
			class GraphicTaskSubSystem
				: public ITaskSubSystem
				, public Engine::GameProcessAnchorObserver::IGraphicPrepareHandler
			{
			public:
				GraphicTaskSubSystem(int initialThreadCount);
				virtual ~GraphicTaskSubSystem();

			public:	// ITaskSubSystem
				virtual void CollectModuleDependency(std::set<EngineModuleIdType> dependencySet) const override;

				virtual void InitializeSubSystem(TaskManagerComponent *ownerManager,
					Application::ApplicationComponent *application, Engine::GameProcessAnchor *gameAnchor) override;

				virtual void CreateProcessors(std::function<void(std::shared_ptr<ITaskProcessor>)> registerFunc) override;

			public:	// IGraphicPrepareHandler
				virtual void GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare) override;

			protected:
				int m_initialThreadCount;

				TaskManagerComponent *m_ownerManager;
				Application::ApplicationComponent *m_appComponentCopy;
				Engine::GameProcessAnchor *m_gameAnchorCopy;

				std::shared_ptr<GraphicTask::GraphicThreadWorkerPool> m_graphicProcessor;
			};

			//------------------------------------------------------------------------------
			GraphicTaskSubSystem::GraphicTaskSubSystem(int initialThreadCount)
				: m_initialThreadCount(initialThreadCount)
			{
			}

			//------------------------------------------------------------------------------
			GraphicTaskSubSystem::~GraphicTaskSubSystem()
			{
				if (m_gameAnchorCopy != nullptr)
				{
					m_gameAnchorCopy->UnRegisterOnGraphicPrepare(this);
				}
			}

			//------------------------------------------------------------------------------
			void GraphicTaskSubSystem::CollectModuleDependency(std::set<EngineModuleIdType> dependencySet) const
			{
				dependencySet.insert(EngineModuleIdType::kGraphics);
			}

			//------------------------------------------------------------------------------
			void GraphicTaskSubSystem::InitializeSubSystem(TaskManagerComponent *ownerManager, Application::ApplicationComponent *application, Engine::GameProcessAnchor *gameAnchor)
			{
				m_ownerManager = ownerManager;
				m_appComponentCopy = application;
				m_gameAnchorCopy = gameAnchor;

				gameAnchor->RegisterOnGraphicPrepare(this);
			}

			//------------------------------------------------------------------------------
			void GraphicTaskSubSystem::CreateProcessors(std::function<void(std::shared_ptr<ITaskProcessor>)> registerFunc)
			{
				m_graphicProcessor = std::make_shared<GraphicTask::GraphicThreadWorkerPool>(m_ownerManager, m_appComponentCopy, m_gameAnchorCopy);
				registerFunc(m_graphicProcessor);
			}

			//------------------------------------------------------------------------------
			void GraphicTaskSubSystem::GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare)
			{
				// Only for first prepare
				if (!isFirstGraphicPrepare)
				{
					return;
				}

				if (!m_graphicProcessor)
				{
					return;
				}

				// Deferred Worker Start to wait GL Context Initialization
				if (m_graphicProcessor->GetWorkerCount() < m_initialThreadCount)
				{
					m_graphicProcessor->IncreaseWorker(m_initialThreadCount);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// Graphic Task System

			//------------------------------------------------------------------------------
			// Enable Graphic Task System for the task manager
			void EnableGraphicTaskSystem(TaskManagerComponent *taskManager, int graphicThreadCount)
			{
				if (graphicThreadCount > 128)
				{
					// Insane count
					graphicThreadCount = 128;
				}
				else if (graphicThreadCount <= 0)
				{
					// Minimum 1 thread needed
					graphicThreadCount = 1;
				}

				taskManager->AttachSubSystem(std::make_shared<GraphicTaskSubSystem>(graphicThreadCount));
			}
		}
	}
}
