////////////////////////////////////////////////////////////////////////////////
// Game.h (${{ProgramName}} - ${{ProgramName}})
//
// Game Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __${{ProgramName-Upper}}__GAME_H
#define __${{ProgramName-Upper}}__GAME_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Engine
#include <Engine/Toolbox/ModuledGame/ModuledGame.h>


namespace ${{ProgramName}}
{
	// Real Game Class of ${{ProgramName}} Project
	class Game
		: public Leggiero::Toolbox::Game::ModuledGame
	{
	public:
		Game();
		virtual ~Game();

	public:	// BaseGame
		virtual bool _UpdateFrame(Leggiero::GameTimeClockType::time_point frameTime) override;

		virtual void _Prepare() override;
		virtual void _GraphicPrepare() override;

		virtual void _GraphicShutdown() override;
		virtual void _Shutdown() override;

	public:	// ModuledGame
		virtual void _InitializeUsingModules(Leggiero::IPlatformApplication *application) override;
		virtual void _RegisterUsingComponents() override;

	protected:
		void _FinalizeGameScenes();

	};
}

#endif
