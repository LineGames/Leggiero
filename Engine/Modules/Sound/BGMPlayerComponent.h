////////////////////////////////////////////////////////////////////////////////
// BGMPlayerComponent.h (Leggiero/Modules - Sound)
//
// Platform Supported Music Player Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__BGM_PLAYER_COMPONENT_H
#define __LM_SOUND__BGM_PLAYER_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <string>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.Sound
#include "Common/SoundTypes.h"
#include "Common/IBGMPlayingHandle.h"


namespace Leggiero
{
	namespace Sound
	{
		// BGM Player Component
		class BGMPlayerComponent
			: public EngineComponent
		{
		public:	// Can be created by Creation Function (for each platform)
			static BGMPlayerComponent *CreateComponentObject(bool isUseBundleFileSystem = true);

		public:
			BGMPlayerComponent();
			virtual ~BGMPlayerComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kBGMPlayer; };

		public:
			virtual std::shared_ptr<IBGMPlayingHandle> PlayFromFile(const std::string &filePath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) = 0;
			virtual std::shared_ptr<IBGMPlayingHandle> PlayInBundle(const std::string &virtualPath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) = 0;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Sound::BGMPlayerComponent, Leggiero::EngineComponentIdType::kBGMPlayer);

#endif
