////////////////////////////////////////////////////////////////////////////////
// Scene/Game_Scenes.cpp (${{ProgramName}} - ${{ProgramName}})
//
// Implementation of Game Class's Scene related parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "../Game.h"

// Leggiero.Engine
#include <Engine/Toolbox/Scene/IGameScene.h>

// ${{ProgramName}}
#include "SceneIds.h"
#include "DefaultUIScene/DefaultUIScene.h"


namespace ${{ProgramName}}
{
	//////////////////////////////////////////////////////////////////////////////// Game - Scenes

	//------------------------------------------------------------------------------
	void Game::_PrepareGameScenes()
	{
		DefaultUISceneStuffs::DefaultUIScene *defaultScene = new DefaultUISceneStuffs::DefaultUIScene(this, this);
		RegisterGameScene(defaultScene);

		// Start Scene
		RequestSceneChange(SceneIds::kDefault);
	}

	//------------------------------------------------------------------------------
	void Game::_FinalizeGameScenes()
	{
		std::vector<Leggiero::Toolbox::Scene::IGameScene *> allScenes = GetAllRegisteredScenes();
		for (Leggiero::Toolbox::Scene::IGameScene *deletingScene : allScenes)
		{
			UnRegisterGameScene(deletingScene->GetSceneId());
			delete deletingScene;
		}
	}
}
