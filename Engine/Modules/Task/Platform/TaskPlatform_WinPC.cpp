////////////////////////////////////////////////////////////////////////////////
// Platform/TaskPlatform_WinPC.cpp (Leggiero/Modules - Task)
//
// Task Implementation for WinPC Platform
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Task
#include "../_Internal/_ConcreteTaskManager.h"
#include "../Processor/ThreadWorkerPool.h"


namespace Leggiero
{
	namespace Task
	{
		namespace _Internal
		{
			//////////////////////////////////////////////////////////////////////////////// ConcreteTaskManager

			//------------------------------------------------------------------------------
			std::shared_ptr<ITaskProcessor> ConcreteTaskManager::_CreateGeneralProcessor()
			{
				// Just use General Worker Pool
				std::shared_ptr<ThreadWorkerPool> createdWorkerPool = std::make_shared<ThreadWorkerPool>(this, m_appComponentCopy);
				if (!createdWorkerPool)
				{
					return nullptr;
				}

				// Default Pool Size = 4
				if (createdWorkerPool->IncreaseWorker(4) == 0)
				{
					// Cannot start worker
					return nullptr;
				}

				return createdWorkerPool;
			}
		}
	}
}
