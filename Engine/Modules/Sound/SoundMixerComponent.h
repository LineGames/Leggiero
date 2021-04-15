////////////////////////////////////////////////////////////////////////////////
// SoundMixerComponent.h (Leggiero/Modules - Sound)
//
// Engine Sound Mixer Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__SOUND_MIXER_COMPONENT_H
#define __LM_SOUND__SOUND_MIXER_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.Sound
#include "Common/SoundTypes.h"
#include "Common/SoundPlayingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		// Forward Declaration
		class ISoundProvider;


		// Sound Mixer Component
		class SoundMixerComponent
			: public EngineComponent
		{
		public:	// Can be created by Creation Function (for each platform sound mixers)
			static SoundMixerComponent *CreateComponentObject();

		public:
			SoundMixerComponent();
			virtual ~SoundMixerComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kSoundMixer; };

		public:
			virtual std::shared_ptr<SoundPlayingContext> Play(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately = true, float volume = 1.0f, bool isPauseAfterFinish = false) = 0;
			virtual std::shared_ptr<LoopingSoundPlayingContext> PlayLoopingSound(std::shared_ptr<ISoundProvider> sound, SampleNumberType loopStartSamplePosition, SampleNumberType loopFinishSamplePosition, bool isStartImmediately = true, float volume = 1.0f) = 0;
			virtual std::shared_ptr<BufferedSoundPlayingContext> PlayBufferedSound(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately = true, float volume = 1.0f, bool isLooping = false, bool isPauseAfterFinish = false) = 0;

			virtual void ClearNonManagedSounds() = 0;

		public:
			virtual bool IsSuspended() const { return false; }
			virtual void HintUpdate() { }
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::Sound::SoundMixerComponent, Leggiero::EngineComponentIdType::kSoundMixer);

#endif
