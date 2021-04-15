////////////////////////////////////////////////////////////////////////////////
// Scene/DefaultScene/DefaultScene.cpp (${{ProgramName}} - ${{ProgramName}})
//
// Default Game Scene Source Code
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "DefaultScene.h"


namespace ${{ProgramName}}
{
	namespace DefaultSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// DefaultScene

		//------------------------------------------------------------------------------
		DefaultScene::DefaultScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game)
			: Leggiero::Toolbox::Scene::IGameScene(ownerContext)
			, m_game(game)
		{

		}

		//------------------------------------------------------------------------------
		DefaultScene::~DefaultScene()
		{

		}

		//------------------------------------------------------------------------------
		// Re-Initialize Scene's Graphic Contents (after scene's graphic contents invalidated)
		// Called in Graphic Thread
		void DefaultScene::ReInitializeSceneGraphic()
		{

		}

		//------------------------------------------------------------------------------
		// Prepare Scene Just Before Enter
		void DefaultScene::BeforeSceneEnter()
		{
			
		}

		//------------------------------------------------------------------------------
		// Process Scene Just After Exit
		void DefaultScene::AfterSceneExit()
		{

		}

		//------------------------------------------------------------------------------
		// Process a Game Frame
		void DefaultScene::ProcessFrame(Leggiero::GameTimeClockType::time_point frameReferenceTime)
		{
			
		}

		//------------------------------------------------------------------------------
		// Initialize Scene Contents
		void DefaultScene::_InitializeScene()
		{

		}

		//------------------------------------------------------------------------------
		// Initialize Scene's Graphic Contents
		// Called in Graphic Thread
		void DefaultScene::_InitializeSceneGraphic()
		{

		}

		//------------------------------------------------------------------------------
		// Finalize Scene Contents
		void DefaultScene::_FinalizeScene()
		{

		}

		//------------------------------------------------------------------------------
		// Finalize Scene's Graphic Contents
		// Called in Graphic Thread
		void DefaultScene::_FinalizeSceneGraphic()
		{
			
		}
	}
}
