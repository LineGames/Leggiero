////////////////////////////////////////////////////////////////////////////////
// Application/BaseGame.cpp (Leggiero - Engine)
//
// Common Game Base Class Implementation
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "BaseGame.h"

// Leggiero.Engine
#include "GameProcessAnchor.h"


namespace Leggiero
{
	namespace Engine
	{
		//////////////////////////////////////////////////////////////////////////////// BaseGame

		//------------------------------------------------------------------------------
		BaseGame::BaseGame()
			: m_isShutdowned(false), m_isFirstGraphicPrepare(true), m_gameFrameCount(0)
		{
			pthread_mutex_init(&m_shutdownMutex, NULL);

			m_processEventAnchor = new GameProcessAnchor();
		}

		//------------------------------------------------------------------------------
		BaseGame::~BaseGame()
		{
			Shutdown();
			pthread_mutex_destroy(&m_shutdownMutex);
		}

		//------------------------------------------------------------------------------
		// Advance a Game Frame
		bool BaseGame::UpdateFrame()
		{
			GameTimeClockType::time_point frameReferenceTime = GameTimeClockType::now();

			++m_gameFrameCount;
			int64_t currentFrameNumber = m_gameFrameCount;

			m_processEventAnchor->NotifyBeforeFrame(currentFrameNumber, frameReferenceTime);

			bool frameResult = _UpdateFrame(frameReferenceTime);

			m_processEventAnchor->NotifyAfterFrame(currentFrameNumber);

			return frameResult;
		}

		//------------------------------------------------------------------------------
		// Return whether the game is running or not
		bool BaseGame::IsRunning() const
		{
			return !m_isShutdowned;
		}

		//------------------------------------------------------------------------------
		// Do preparation needed before start the game loop
		void BaseGame::Prepare()
		{
			m_processEventAnchor->NotifyPrepare();
			_Prepare();
		}

		//------------------------------------------------------------------------------
		// Do finalization before game application exit
		void BaseGame::Shutdown()
		{
			// Prevent duplicated shutdown
			int lockResult = pthread_mutex_lock(&m_shutdownMutex);
			if (lockResult == 0)
			{
				pthread_mutex_t *lockCopy = &m_shutdownMutex;
				auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				if (m_isShutdowned)
				{
					return;
				}
				m_isShutdowned = true;
			}
			else
			{
				// Keep going
				bool isAlreadyShutdowned = m_isShutdowned;
				m_isShutdowned = true;
				if (isAlreadyShutdowned)
				{
					return;
				}
			}

			_Shutdown();

			if (m_processEventAnchor != nullptr)
			{
				m_processEventAnchor->NotifyShutdown();
			}

			// Finalize Engine Resources
			_FinalizeGameEngineComponents();
			_FinalizeGameEngineLibrary();

			// Delete the anchor to release captured resources
			if (m_processEventAnchor != nullptr)
			{
				delete m_processEventAnchor;
				m_processEventAnchor = nullptr;
			}
		}

		//------------------------------------------------------------------------------
		// Do graphic initialization before start the game loop
		// (Ensure the call is in the rendering thread)
		// This function can be called again during game session:
		//   - it means Graphic Context have been recreated after deletion, so you must re-initialize graphic resources
		void BaseGame::GraphicPrepare()
		{
			m_processEventAnchor->NotifyGraphicPrepare(m_isFirstGraphicPrepare);
			m_isFirstGraphicPrepare = false;

			_GraphicPrepare();
		}

		//------------------------------------------------------------------------------
		// Do graphic finalization before game application exit
		// (Ensure the call is in the rendering thread)
		void BaseGame::GraphicShutdown()
		{
			_GraphicShutdown();
			m_processEventAnchor->NotifyGraphicShutdown();
		}
	}
}
