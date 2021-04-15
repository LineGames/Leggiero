////////////////////////////////////////////////////////////////////////////////
// Toolbox/Scene/GameSceneHolder.cpp (Leggiero - Engine)
//
// Game Scene Holder Base Class Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GameSceneHolder.h"

// Leggiero.Engine
#include "IGameScene.h"


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Scene
		{
			//////////////////////////////////////////////////////////////////////////////// GameSceneHolder

			//------------------------------------------------------------------------------
			GameSceneHolder::GameSceneHolder()
			{

			}

			//------------------------------------------------------------------------------
			// Get Registered Scene Object of Given Id
			IGameScene *GameSceneHolder::GetSceneObject(GameSceneIdType sceneId)
			{
				std::unordered_map<GameSceneIdType, IGameScene *>::iterator findIt = m_gameSceneTable.find(sceneId);
				if (findIt != m_gameSceneTable.end())
				{
					return findIt->second;
				}
				return nullptr;
			}

			//------------------------------------------------------------------------------
			bool GameSceneHolder::GameSceneProcessFrame(GameTimeClockType::time_point frameTime)
			{
				if (m_currentScene == nullptr || m_currentScene->GetSceneId() != m_nextSceneId)
				{
					// Scene Transition
					if (m_currentScene != nullptr)
					{
						m_currentScene->AfterSceneExit();
						m_currentScene = nullptr;
					}

					IGameScene *nextScene = GetSceneObject(m_nextSceneId);
					if (nextScene != nullptr)
					{
						if (!nextScene->IsInitialized())
						{
							nextScene->InitializeScene();
						}
						if (!nextScene->IsGraphicInitialized())
						{
							nextScene->InitializeSceneGraphic();
						}

						nextScene->BeforeSceneEnter();
						m_currentScene = nextScene;
					}
				}

				if (m_currentScene == nullptr)
				{
					return false;
				}

				m_currentScene->ProcessFrame(frameTime);

				return true;
			}

			//------------------------------------------------------------------------------
			void GameSceneHolder::GameSceneProcessPrepare()
			{
				_PrepareGameScenes();

				for (std::unordered_map<GameSceneIdType, IGameScene *>::iterator it = m_gameSceneTable.begin(); it != m_gameSceneTable.end(); ++it)
				{
					if (!it->second->IsInitialized())
					{
						it->second->InitializeScene();
					}
				}
			}

			//------------------------------------------------------------------------------
			void GameSceneHolder::GameSceneProcessShutdown()
			{
				if (m_currentScene != nullptr)
				{
					m_currentScene->AfterSceneExit();
					m_currentScene = nullptr;
				}
				for (std::unordered_map<GameSceneIdType, IGameScene *>::iterator it = m_gameSceneTable.begin(); it != m_gameSceneTable.end(); ++it)
				{
					it->second->FinalizeScene();
				}
			}

			//------------------------------------------------------------------------------
			void GameSceneHolder::GameSceneProcessGraphicPrepare()
			{
				for (std::unordered_map<GameSceneIdType, IGameScene *>::iterator it = m_gameSceneTable.begin(); it != m_gameSceneTable.end(); ++it)
				{
					if (it->second->IsGraphicInitialized())
					{
						it->second->ReInitializeSceneGraphic();
					}
					else
					{
						it->second->InitializeSceneGraphic();
					}
				}
			}

			//------------------------------------------------------------------------------
			void GameSceneHolder::GameSceneProcessGraphicShutdown()
			{
				for (std::unordered_map<GameSceneIdType, IGameScene *>::iterator it = m_gameSceneTable.begin(); it != m_gameSceneTable.end(); ++it)
				{
					it->second->FinalizeSceneGraphic();
				}
			}

			//------------------------------------------------------------------------------
			// Register a Scene to the Holder
			// It will manage scene for each events, but it will not delete scene objects
			void GameSceneHolder::RegisterGameScene(IGameScene *scene, bool isDoInitialize)
			{
				if (scene == nullptr)
				{
					return;
				}

				if (isDoInitialize && !scene->IsInitialized())
				{
					scene->InitializeScene();
				}

				GameSceneIdType sceneId = scene->GetSceneId();
				m_gameSceneTable[sceneId] = scene;
			}

			//------------------------------------------------------------------------------
			// UnRegister a Scene
			void GameSceneHolder::UnRegisterGameScene(GameSceneIdType sceneId)
			{
				m_gameSceneTable.erase(sceneId);
			}

			//------------------------------------------------------------------------------
			// Get All Registered Scenes
			std::vector<IGameScene *> GameSceneHolder::GetAllRegisteredScenes()
			{
				std::vector<IGameScene *> allScenes;
				allScenes.reserve(m_gameSceneTable.size());
				for (std::unordered_map<GameSceneIdType, IGameScene *>::iterator it = m_gameSceneTable.begin(); it != m_gameSceneTable.end(); ++it)
				{
					allScenes.push_back(it->second);
				}
				return allScenes;
			}
		}
	}
}
