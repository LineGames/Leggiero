////////////////////////////////////////////////////////////////////////////////
// Platform/AndroidPlatformSound.cpp (Leggiero/Modules - Sound)
//
// Sound Implementation for Android Platform Specific Features
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Sound
#include "../SoundMixerComponent.h"
#include "../OboeBackend/OboeCommon.h"
#include "../OboeBackend/OboeSoundMixer.h"


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// Platform Specific Settings

		namespace Oboe
		{
			namespace Android
			{
				//------------------------------------------------------------------------------
				// Platform Dependent Sound Mixer
				class AndroidOboeSoundMixer : public OboeSoundMixer
				{
				public:
					AndroidOboeSoundMixer() { }
					virtual ~AndroidOboeSoundMixer() { }

				public:	// Manual suspend to save resource during background
					virtual void OnGoToBackground() override { _SuspendMixer(); }
					virtual void OnReturnFromBackground() override { _RestoreMixer(); }
				};
			}

			//------------------------------------------------------------------------------
			// Data Buffer Default Size
			size_t GetDataBufferDefaultSize()
			{
				return 8192;
			}

			//------------------------------------------------------------------------------
			long OboeSoundMixer::_GetUpdateThreadSleepTimeInNS() const
			{
				return 5 * 1000 * 1000;	// 5 ms
			}
		}


		//////////////////////////////////////////////////////////////////////////////// SoundMixerComponent

		//------------------------------------------------------------------------------
		// Sound Mixer Component Factory Function
		SoundMixerComponent *SoundMixerComponent::CreateComponentObject()
		{
			return new Oboe::Android::AndroidOboeSoundMixer();
		}
	}
}
