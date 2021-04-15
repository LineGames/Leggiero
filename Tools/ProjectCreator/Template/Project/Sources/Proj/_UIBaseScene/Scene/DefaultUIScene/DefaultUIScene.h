////////////////////////////////////////////////////////////////////////////////
// Scene/DefaultUIScene/DefaultUIScene.h (${{ProgramName}} - ${{ProgramName}})
//
// Default Game Scene with UI Functionality from Leggiero Legacy UI
////////////////////////////////////////////////////////////////////////////////

#ifndef __${{ProgramName-Upper}}__SCENE__DEFAULT_UI_SCENE__DEFAULT_UI_SCENE_H
#define __${{ProgramName-Upper}}__SCENE__DEFAULT_UI_SCENE__DEFAULT_UI_SCENE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.LegacyUI
#include <LUI/Scene/UISceneBase.h>

// ${{ProgramName}}
#include "../SceneIds.h"


namespace ${{ProgramName}}
{
	namespace DefaultUISceneStuffs
	{
		// Empty Default Game Scene
		class DefaultUIScene
			: public Leggiero::LUI::UISceneBase
		{
		public:
			DefaultUIScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game);
			virtual ~DefaultUIScene();

		public:	// IGameScene
			// Get Id of the Scene
			virtual Leggiero::Toolbox::Scene::GameSceneIdType GetSceneId() const override { return SceneIds::kDefault; }

		protected:	// UISceneBase
			virtual void _InitializeUIContents() override;
			virtual void _FinalizeUIContents() override;

			virtual void _SetUIBeforeEnterFrame() override;
			virtual void _UnSetUIAfterExitFrame() override;

			virtual void _ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval) override;
		};
	}
}

#endif
