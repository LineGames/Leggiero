////////////////////////////////////////////////////////////////////////////////
// BGMPlayerComponent.cpp (Leggiero/Modules - Sound)
//
// BGM Player Component Base Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "BGMPlayerComponent.h"


DEFINE_GET_COMPONENT_INTERFACE(Leggiero::Sound::BGMPlayerComponent, Leggiero::EngineComponentIdType::kBGMPlayer);


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// BGMPlayerComponent

		//------------------------------------------------------------------------------
		BGMPlayerComponent::BGMPlayerComponent()
		{
		}

		//------------------------------------------------------------------------------
		BGMPlayerComponent::~BGMPlayerComponent()
		{
		}
	}
}
