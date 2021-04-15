////////////////////////////////////////////////////////////////////////////////
// Processor/iOSTaskProcessor.h (Leggiero/Modules - Task)
//
// iOS Platform General Task Processor using GCD
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__PROCESSOR__IOS_TASK_PROCESSOR_H
#define __LM_TASK__PROCESSOR__IOS_TASK_PROCESSOR_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>

// Leggiero.Task
#include "../TaskTypes.h"
#include "ITaskProcessor.h"


namespace Leggiero
{
	namespace Task
	{
		// Forward Declarations
		class ITaskManagerSystemFunctions;
		struct TaskExecutionEntry;

		namespace iOS
		{
			// iOS Task Processor
			class IOSTaskProcessor
				: public ITaskProcessor
			{
			public:
				IOSTaskProcessor(ITaskManagerSystemFunctions *pManager);
				virtual ~IOSTaskProcessor();

			public:	// ITaskProcessor
				// Give job to processor
				virtual void GiveJob(TaskExecutionEntry *job) override;

				// Get task capability of the processor
				virtual TaskCapabilityType GetProcessorTaskCapability() override { return TaskCapabilities::kGeneral; }

				// Is processor handle sleeps of its tasks?
				virtual bool IsManagingSleeps() override { return true; }
				
				virtual void PrepareProcessorShutdown() override;
				
			protected:
				ITaskManagerSystemFunctions *m_pManager;
				
				std::shared_ptr<Utility::Object::PointerHolder> m_myPointerHolder;
			};
		}
	}
}

#endif
