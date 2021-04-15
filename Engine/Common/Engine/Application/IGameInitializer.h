////////////////////////////////////////////////////////////////////////////////
// Application/IGameInitializer.h (Leggiero - Engine)
//
// Interface for Initializer part of the Game
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__I_GAME_INITIALIZER_H
#define __ENGINE__APPLICATION__I_GAME_INITIALIZER_H


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


	// Interface for game creation
	class IGameInitializer
	{
	public:
		IGameInitializer() { }
		virtual ~IGameInitializer() { }

		friend std::shared_ptr<IGame> CreateGame(IPlatformApplication *application, IGameLaunchOption *launchOption);

	// Leggiero Engine Module and Component Initializer
	protected:
		virtual void _InitializeGameEngineLibrary(IPlatformApplication *application) = 0;
		virtual void _FinalizeGameEngineLibrary() = 0;

		virtual void _AssembleGameEngineComponents(IPlatformApplication *application) = 0;
		virtual void _FinalizeGameEngineComponents() = 0;
	};
}

#endif
