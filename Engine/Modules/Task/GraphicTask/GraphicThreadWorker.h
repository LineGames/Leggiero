////////////////////////////////////////////////////////////////////////////////
// GraphicTass/GraphicThreadWorker.h (Leggiero/Modules - Task)
//
// Thread Worker with Graphic Context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__GRAPHIC_TASK__GRAPHIC_THREAD_WORKER_H
#define __LM_TASK__GRAPHIC_TASK__GRAPHIC_THREAD_WORKER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <memory>

// Leggiero.Engine
#include <Engine/Application/GameProcessAnchorObserver.h>

// Leggiero.Task
#include "../Processor/ThreadWorker.h"


namespace Leggiero
{
	// Forward Declarations
	namespace Engine
	{
		class GameProcessAnchor;
	}

	namespace Graphics
	{
		class IGLThreadContextInformation;
	}


	namespace Task
	{
		namespace GraphicTask
		{
			// Graphic Thread Worker
			class GraphicThreadWorker
				: public ThreadWorker
				, public Engine::GameProcessAnchorObserver::IGraphicPrepareHandler
			{
			public:
				GraphicThreadWorker(IThreadWorkerContext *pOwnerContext, Engine::GameProcessAnchor *gameAnchor);
				virtual ~GraphicThreadWorker();

			protected:	// ThreadWorker
				virtual bool _InitializeBeforeWork() override;
				virtual void _FinalizeAfterWork() override;

				virtual void _PostProcessAfterTask() override;

				virtual bool _LoopProcess() override;

			public:	// IGraphicPrepareHandler
				virtual void GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare) override;

			protected:
				Engine::GameProcessAnchor *m_gameAnchor;
				bool m_isSubscribed;

				std::shared_ptr<Graphics::IGLThreadContextInformation> m_graphicsContext;

				std::atomic_bool m_checkAndRefreshRequested;
			};
		}
	}
}

#endif
