////////////////////////////////////////////////////////////////////////////////
// Toolbox/Scene/IGameScene.h (Leggiero - Engine)
//
// Game Scene Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__TOOLBOX__SCENE__I_GAME_SCENE_H
#define __ENGINE__TOOLBOX__SCENE__I_GAME_SCENE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Engine
#include "GameSceneTypes.h"


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Scene
		{
			// Forward Declaration of Scene Context Interface
			class IGameSceneContext;


			// Interface for Game Scene
			class IGameScene
			{
			public:
				IGameScene(IGameSceneContext *ownerContext)
					: m_ownerContext(ownerContext)
					, m_isInitialized(false), m_isGraphicInitialized(false)
				{ }

				virtual ~IGameScene() { }

			private:
				// All scenes have to have their context
				IGameScene() = delete;

			public:
				// Get Id of the Scene
				virtual GameSceneIdType GetSceneId() const { return kInvalidSceneId; }

				// Initialize Scene Contents
				void InitializeScene()
				{
					_InitializeScene();
					m_isInitialized = true;
				}

				// Initialize Scene's Graphic Contents
				// Called in Graphic Thread
				void InitializeSceneGraphic()
				{
					_InitializeSceneGraphic();
					m_isGraphicInitialized = true;
				}

				// Re-Initialize Scene's Graphic Contents (after scene's graphic contents invalidated)
				// Called in Graphic Thread
				virtual void ReInitializeSceneGraphic() { }

				// Finalize Scene Contents
				void FinalizeScene()
				{
					_FinalizeScene();
					m_isInitialized = false;
				}

				// Finalize Scene's Graphic Contents
				// Called in Graphic Thread
				void FinalizeSceneGraphic()
				{
					_FinalizeSceneGraphic();
					m_isGraphicInitialized = false;
				}

				// Prepare Scene Just Before Enter
				virtual void BeforeSceneEnter() { }

				// Process Scene Just After Exit
				virtual void AfterSceneExit() { }

				// Process a Game Frame
				virtual void ProcessFrame(GameTimeClockType::time_point frameReferenceTime) = 0;

				// Check whether if game scene's initialization have done or not
				bool IsInitialized() { return m_isInitialized; }

				// Check whether if game scene's graphical initialization have done or not
				bool IsGraphicInitialized() { return m_isGraphicInitialized; }

			protected:
				// Initialize Scene Contents
				virtual void _InitializeScene() { }

				// Initialize Scene's Graphic Contents
				// Called in Graphic Thread
				virtual void _InitializeSceneGraphic() { }

				// Finalize Scene Contents
				virtual void _FinalizeScene() { }

				// Finalize Scene's Graphic Contents
				// Called in Graphic Thread
				virtual void _FinalizeSceneGraphic() { }

			protected:
				IGameSceneContext	*m_ownerContext;

				bool				m_isInitialized;
				bool				m_isGraphicInitialized;
			};
		}
	}
}

#endif
