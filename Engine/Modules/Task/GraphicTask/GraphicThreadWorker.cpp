////////////////////////////////////////////////////////////////////////////////
// GraphicTask/GraphicThreadWorker.cpp (Leggiero/Modules - Task)
//
// Graphic Thread Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GraphicThreadWorker.h"

// External Library
#include <GLES3.h>

// Leggiero.Engine
#include <Engine/Application/GameProcessAnchor.h>

// Leggiero.Graphics
#include <Graphics/GraphicsThreadContext.h>


namespace Leggiero
{
	namespace Task
	{
		namespace GraphicTask
		{
			//////////////////////////////////////////////////////////////////////////////// GraphicThreadWorker

			//------------------------------------------------------------------------------
			GraphicThreadWorker::GraphicThreadWorker(IThreadWorkerContext *pOwnerContext, Engine::GameProcessAnchor *gameAnchor)
				: ThreadWorker(pOwnerContext), m_gameAnchor(gameAnchor)
				, m_isSubscribed(false), m_checkAndRefreshRequested(false)
			{
			}

			//------------------------------------------------------------------------------
			GraphicThreadWorker::~GraphicThreadWorker()
			{
				if (m_isSubscribed && m_gameAnchor != nullptr)
				{
					m_gameAnchor->UnRegisterOnGraphicPrepare(this);
				}
			}

			//------------------------------------------------------------------------------
			bool GraphicThreadWorker::_InitializeBeforeWork()
			{
				m_graphicsContext = Graphics::MakeThreadGLContext();
				if (!m_graphicsContext)
				{
					return false;
				}

				m_gameAnchor->RegisterOnGraphicPrepare(this);
				m_isSubscribed = true;

				return true;
			}

			//------------------------------------------------------------------------------
			void GraphicThreadWorker::_FinalizeAfterWork()
			{
				m_gameAnchor->UnRegisterOnGraphicPrepare(this);
				m_isSubscribed = false;

				std::shared_ptr<Graphics::IGLThreadContextInformation> contextCopy = m_graphicsContext;
				m_graphicsContext.reset();

				if (contextCopy)
				{
					Graphics::TerminateThreadGLContext(contextCopy);
				}
			}

			//------------------------------------------------------------------------------
			void GraphicThreadWorker::_PostProcessAfterTask()
			{
				glFinish();
			}

			//------------------------------------------------------------------------------
			bool GraphicThreadWorker::_LoopProcess()
			{
				if (m_checkAndRefreshRequested.load())
				{
					if (m_graphicsContext)
					{
						Graphics::CheckAndRefreshThreadGLContext(m_graphicsContext);
					}
					m_checkAndRefreshRequested.store(false);
				}

				return true;
			}

			//------------------------------------------------------------------------------
			void GraphicThreadWorker::GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare)
			{
				m_checkAndRefreshRequested.store(true);
			}
		}
	}
}
