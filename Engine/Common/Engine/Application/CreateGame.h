////////////////////////////////////////////////////////////////////////////////
// Application/CreateGame.h (Leggiero - Engine)
//
// Game creation interface definition
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__CREATE_GAME_H
#define __ENGINE__APPLICATION__CREATE_GAME_H


// Standrad Library
#include <memory>


namespace Leggiero
{
	// Forward declaration of the game interface
	class IGame;

	// Forward declaration of platform application
	class IPlatformApplication;

	// Forward declaration of game launch option
	class IGameLaunchOption;


	// Create a Game Object for given Application
	// Each game project should implement this function to create the Concrete Game
	extern std::shared_ptr<IGame> CreateGame(IPlatformApplication *application, IGameLaunchOption *launchOption);
}

#endif
