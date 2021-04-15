////////////////////////////////////////////////////////////////////////////////
// Platform/TaskPlatform_iOS.mm (Leggiero/Modules - Task)
//
// Task Implementation for iOS Platform
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Task
#include "../_Internal/_ConcreteTaskManager.h"
#include "../Processor/iOSTaskProcessor.h"


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
				// Use GCD based task processor
				std::shared_ptr<iOS::IOSTaskProcessor> createdProcessor = std::make_shared<iOS::IOSTaskProcessor>(this);
				if (!createdProcessor)
				{
					return nullptr;
				}
				
				return createdProcessor;
			}
		}
	}
}
