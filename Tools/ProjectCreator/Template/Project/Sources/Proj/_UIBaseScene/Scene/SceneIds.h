////////////////////////////////////////////////////////////////////////////////
// Scene/SceneIds.h (${{ProgramName}} - ${{ProgramName}})
//
// ${{ProgramName}} Project Scene Id Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __${{ProgramName-Upper}}__SCENE__SCENE_IDS_H
#define __${{ProgramName-Upper}}__SCENE__SCENE_IDS_H


// Leggiero.Engine
#include <Engine/Toolbox/Scene/GameSceneTypes.h>


namespace ${{ProgramName}}
{
	class SceneIds
	{
	public:
		using IdType = Leggiero::Toolbox::Scene::GameSceneIdType;

		static constexpr IdType kINVALID = Leggiero::Toolbox::Scene::kInvalidSceneId;

		static constexpr IdType kDefault = 1;
	};
}

#endif
