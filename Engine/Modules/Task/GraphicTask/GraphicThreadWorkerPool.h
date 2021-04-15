////////////////////////////////////////////////////////////////////////////////
// GraphicTass/GraphicThreadWorkerPool.h (Leggiero/Modules - Task)
//
// Graphic task worker pool
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__GRAPHIC_TASK__GRAPHIC_THREAD_WORKER_POOL_H
#define __LM_TASK__GRAPHIC_TASK__GRAPHIC_THREAD_WORKER_POOL_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Task
#include "../Processor/ThreadWorkerPool.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Engine
	{
		class GameProcessAnchor;
	}

	namespace Task
	{
		// Forward Declaration
		class ITaskManagerSystemFunctions;


		namespace GraphicTask
		{
			// Graphic Thread Worker
			class GraphicThreadWorkerPool
				: public ThreadWorkerPool
			{
			public:
				GraphicThreadWorkerPool(ITaskManagerSystemFunctions *pManager, Application::ApplicationComponent *appComponent, Engine::GameProcessAnchor *gameAnchor);
				virtual ~GraphicThreadWorkerPool();

			public:	// ITaskProcessor
				// Get task capability of the processor
				virtual TaskCapabilityType GetProcessorTaskCapability() override { return TaskCapabilities::kGraphics; }

			protected:	// ThreadWorkerPool
				virtual ThreadWorker *_CreateWorker() override;

			protected:
				Engine::GameProcessAnchor *m_gameAnchor;
			};
		}
	}
}

#endif
