////////////////////////////////////////////////////////////////////////////////
// Toolbox/Scene/GameSceneHolder.h (Leggiero - Engine)
//
// Game Scene Holder Base Class
//
// This class is not for thread race condition; mainly considered use in the
// main thread context.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__TOOLBOX__SCENE__GAME_SCENE_HOLDER_H
#define __ENGINE__TOOLBOX__SCENE__GAME_SCENE_HOLDER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <unordered_map>
#include <vector>

// Leggiero.Engine
#include "GameSceneTypes.h"
#include "IGameSceneContext.h"


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Scene
		{
			// Forward Declaration of Scene Interface
			class IGameScene;


			// Base Class for Game Scene Holder
			class GameSceneHolder
				: public IGameSceneContext
			{
			public:
				GameSceneHolder();
				virtual ~GameSceneHolder() { }

			public:	// IGameSceneContext
				// Get Registered Scene Object of Given Id
				virtual IGameScene *GetSceneObject(GameSceneIdType sceneId) override;

			public:
				virtual bool GameSceneProcessFrame(GameTimeClockType::time_point frameTime);

				virtual void GameSceneProcessPrepare();
				virtual void GameSceneProcessShutdown();

				virtual void GameSceneProcessGraphicPrepare();
				virtual void GameSceneProcessGraphicShutdown();

			public:
				// Register a Scene to the Holder
				// It will manage scene for each events, but it will not delete scene objects
				virtual void RegisterGameScene(IGameScene *scene, bool isDoInitialize = true);

				// UnRegister a Scene
				virtual void UnRegisterGameScene(GameSceneIdType sceneId);

				// Get All Registered Scenes
				virtual std::vector<IGameScene *> GetAllRegisteredScenes();

			protected:
				virtual void _PrepareGameScenes() { }

			protected:
				std::unordered_map<GameSceneIdType, IGameScene *> m_gameSceneTable;
			};
		}
	}
}

#endif
