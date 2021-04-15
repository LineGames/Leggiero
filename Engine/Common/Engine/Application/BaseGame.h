////////////////////////////////////////////////////////////////////////////////
// Application/BaseGame.h (Leggiero - Engine)
//
// Game Base Class with Common Basic Implementations
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__BASE_GAME_H
#define __ENGINE__APPLICATION__BASE_GAME_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>

// External Library
#include <pthread.h>

// Leggiero.Engine
#include "IGame.h"


namespace Leggiero
{
	namespace Engine
	{
		// Forward Declarations
		class GameProcessAnchor;


		// Common Base class for the Game
		class BaseGame
			: public IGame
		{
		public:
			BaseGame();
			virtual ~BaseGame();

		public:	// IGame
			// Advance a Game Frame
			virtual bool UpdateFrame() override;

			// Return whether the game is running or not
			virtual bool IsRunning() const override;

			// Do preparation needed before start the game loop
			virtual void Prepare() override;

			// Do finalization before game application exit
			virtual void Shutdown() override;

			// Do graphic initialization before start the game loop
			// (Ensure the call is in the rendering thread)
			// This function can be called again during game session:
			//   - it means Graphic Context have been recreated after deletion, so you must re-initialize graphic resources
			virtual void GraphicPrepare() override;

			// Do graphic finalization before game application exit
			// (Ensure the call is in the rendering thread)
			virtual void GraphicShutdown() override;

		protected:	// For Concrete Game Implementation
			virtual bool _UpdateFrame(GameTimeClockType::time_point frameTime) = 0;

			virtual void _Prepare() = 0;
			virtual void _GraphicPrepare() = 0;

			virtual void _GraphicShutdown() = 0;
			virtual void _Shutdown() = 0;

		private:
			bool			m_isShutdowned;
			pthread_mutex_t	m_shutdownMutex;

			bool			m_isFirstGraphicPrepare;

			int64_t			m_gameFrameCount;

		protected:
			GameProcessAnchor *m_processEventAnchor;
		};
	}
}

#endif
