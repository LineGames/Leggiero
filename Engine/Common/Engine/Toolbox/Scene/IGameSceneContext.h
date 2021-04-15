////////////////////////////////////////////////////////////////////////////////
// Toolbox/Scene/IGameSceneContext.h (Leggiero - Engine)
//
// Game Scene Processing Context Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__TOOLBOX__SCENE__I_GAME_SCENE_CONTEXT_H
#define __ENGINE__TOOLBOX__SCENE__I_GAME_SCENE_CONTEXT_H


// Leggiero.Engine
#include "GameSceneTypes.h"


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Scene
		{
			// Forward Declaration of Scene Interface
			class IGameScene;


			// Interface for Game Scene Context
			class IGameSceneContext
			{
			public:
				IGameSceneContext()
					: m_currentScene(nullptr), m_nextSceneId(kInvalidSceneId)
				{ }
				virtual ~IGameSceneContext() { }

			public:
				// Request Scene Change
				// Scene expected to be changed at next frame
				virtual void RequestSceneChange(GameSceneIdType nextSceneId) { m_nextSceneId = nextSceneId; }

				// Get Current Scene Object
				virtual IGameScene *GetCurrentSceneObject() { return m_currentScene; }

				// Get Registered Scene Object of Given Id
				virtual IGameScene *GetSceneObject(GameSceneIdType sceneId) = 0;

			protected:
				IGameScene			*m_currentScene;
				GameSceneIdType		m_nextSceneId;
			};
		}
	}
}

#endif
