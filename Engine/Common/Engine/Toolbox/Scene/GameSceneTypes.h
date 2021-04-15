////////////////////////////////////////////////////////////////////////////////
// Toolbox/Scene/GameSceneTypes.h (Leggiero - Engine)
//
// Game Scene Related Type Definition
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__TOOLBOX__SCENE__GAME_SCENE_TYPES_H
#define __ENGINE__TOOLBOX__SCENE__GAME_SCENE_TYPES_H


namespace Leggiero
{
	namespace Toolbox
	{
		namespace Scene
		{
			// Game Scene Id Type
			using GameSceneIdType = int;

			// Invalid Scene Id Constant
			constexpr GameSceneIdType kInvalidSceneId = static_cast<GameSceneIdType>(0);
		}
	}
}

#endif
