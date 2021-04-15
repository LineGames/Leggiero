////////////////////////////////////////////////////////////////////////////////
// GraphicTask/GraphicThreadWorkerPool.cpp (Leggiero/Modules - Task)
//
// Graphic Thread Worker Pool Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GraphicThreadWorkerPool.h"

// Leggiero.Task
#include "GraphicThreadWorker.h"


namespace Leggiero
{
	namespace Task
	{
		namespace GraphicTask
		{
			//////////////////////////////////////////////////////////////////////////////// GraphicThreadWorkerPool

			//------------------------------------------------------------------------------
			GraphicThreadWorkerPool::GraphicThreadWorkerPool(ITaskManagerSystemFunctions *pManager, Application::ApplicationComponent *appComponent, Engine::GameProcessAnchor *gameAnchor)
				: ThreadWorkerPool(pManager, appComponent)
				, m_gameAnchor(gameAnchor)
			{
			}

			//------------------------------------------------------------------------------
			GraphicThreadWorkerPool::~GraphicThreadWorkerPool()
			{
			}

			//------------------------------------------------------------------------------
			ThreadWorker *GraphicThreadWorkerPool::_CreateWorker()
			{
				GraphicThreadWorker *createdWorker = new GraphicThreadWorker(this, m_gameAnchor);
				if (createdWorker == nullptr || !createdWorker->IsRunning())
				{
					delete createdWorker;
					return nullptr;
				}
				return createdWorker;
			}
		}
	}
}
