////////////////////////////////////////////////////////////////////////////////
// Scene/DefaultScene/DefaultScene.h (${{ProgramName}} - ${{ProgramName}})
//
// Empty Default Game Scene
////////////////////////////////////////////////////////////////////////////////

#ifndef __${{ProgramName-Upper}}__SCENE__DEFAULT_SCENE__DEFAULT_SCENE_H
#define __${{ProgramName-Upper}}__SCENE__DEFAULT_SCENE__DEFAULT_SCENE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Engine
#include <Engine/Application/IGame.h>
#include <Engine/Toolbox/Scene/IGameScene.h>

// ${{ProgramName}}
#include "../SceneIds.h"


namespace ${{ProgramName}}
{
	namespace DefaultSceneStuffs
	{
		// Empty Default Game Scene
		class DefaultScene
			: public Leggiero::Toolbox::Scene::IGameScene
		{
		public:
			DefaultScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game);
			virtual ~DefaultScene();

		public:	// IGameScene
			// Get Id of the Scene
			virtual Leggiero::Toolbox::Scene::GameSceneIdType GetSceneId() const override { return SceneIds::kDefault; }

			// Re-Initialize Scene's Graphic Contents (after scene's graphic contents invalidated)
			// Called in Graphic Thread
			virtual void ReInitializeSceneGraphic() override;

			// Prepare Scene Just Before Enter
			virtual void BeforeSceneEnter() override;

			// Process Scene Just After Exit
			virtual void AfterSceneExit() override;

			// Process a Game Frame
			virtual void ProcessFrame(Leggiero::GameTimeClockType::time_point frameReferenceTime) override;

		protected:	// IGameScene
			// Initialize Scene Contents
			virtual void _InitializeScene() override;

			// Initialize Scene's Graphic Contents
			// Called in Graphic Thread
			virtual void _InitializeSceneGraphic() override;

			// Finalize Scene Contents
			virtual void _FinalizeScene() override;

			// Finalize Scene's Graphic Contents
			// Called in Graphic Thread
			virtual void _FinalizeSceneGraphic() override;

		protected:
			Leggiero::IGame *m_game;
		};
	}
}

#endif
