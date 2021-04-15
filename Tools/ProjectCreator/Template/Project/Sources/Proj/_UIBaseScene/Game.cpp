////////////////////////////////////////////////////////////////////////////////
// Game.cpp (${{ProgramName}} - ${{ProgramName}})
//
// Implementation of Game Class
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Game.h"


namespace ${{ProgramName}}
{
	//////////////////////////////////////////////////////////////////////////////// Game

	//------------------------------------------------------------------------------
	Game::Game()
	{
	}

	//------------------------------------------------------------------------------
	Game::~Game()
	{
	}

	//------------------------------------------------------------------------------
	bool Game::_UpdateFrame(Leggiero::GameTimeClockType::time_point frameTime)
	{
		if (!GameSceneProcessFrame(frameTime))
		{
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------
	void Game::_Prepare()
	{
		GameSceneProcessPrepare();
	}

	//------------------------------------------------------------------------------
	void Game::_GraphicPrepare()
	{
		GameSceneProcessGraphicPrepare();
	}

	//------------------------------------------------------------------------------
	void Game::_GraphicShutdown()
	{
		GameSceneProcessGraphicShutdown();
	}

	//------------------------------------------------------------------------------
	void Game::_Shutdown()
	{
		GameSceneProcessShutdown();
		_FinalizeGameScenes();
	}
}
