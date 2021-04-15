////////////////////////////////////////////////////////////////////////////////
// Application/IGame.h (Leggiero - Engine)
//
// Interface for Game Object run on Leggiero Engine
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__I_GAME_H
#define __ENGINE__APPLICATION__I_GAME_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Engine
#include "../Module/EngineComponentHolder.h"
#include "IGameInitializer.h"
#include "CreateGame.h"


namespace Leggiero
{
	// Game Class Interface on Leggiero Engine
	class IGame 
		: public EngineComponentHolder
		, public IGameInitializer
		, private Utility::SyntacticSugar::NonCopyable
	{
	public:
		IGame() { }
		virtual ~IGame() { }

	public:
		// Advance a Game Frame
		virtual bool UpdateFrame() = 0;

		// Return whether the game is running or not
		virtual bool IsRunning() const = 0;

		// Do preparation needed before start the game loop
		//  - re-prepare and reuse of a game object is prohibited: create new game object to reset the game
		virtual void Prepare() { }

		// Do finalization before game application exit
		virtual void Shutdown() { }

		// Do graphic initialization before start the game loop
		// (Ensure the call is in the rendering thread)
		// This function can be called again during game session:
		//   - it means Graphic Context have been recreated after deletion, so you must re-initialize graphic resources
		virtual void GraphicPrepare() { }

		// Do graphic finalization before game application exit
		// (Ensure the call is in the rendering thread)
		virtual void GraphicShutdown() { }
	};
}

#endif
