////////////////////////////////////////////////////////////////////////////////
// Platform/WinPCPlatformSound.cpp (Leggiero/Modules - Sound)
//
// Sound Implementation for WinPC Platform Specific Features
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Sound
#include "../SoundMixerComponent.h"
#include "../OpenALBackend/OpenALSoundMixer.h"


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// SoundMixerComponent

		//------------------------------------------------------------------------------
		// Sound Mixer Component Factory Function
		SoundMixerComponent *SoundMixerComponent::CreateComponentObject()
		{
			return new OpenAL::OpenALSoundMixer();
		}


		//////////////////////////////////////////////////////////////////////////////// Platform Specific Settings

		namespace OpenAL
		{
			//------------------------------------------------------------------------------
			size_t OpenALSoundPlayingContext::g_bufferSize = 4096;

			//------------------------------------------------------------------------------
			double OpenALSoundMixer::GetMixerExpectedPlayDelayRatio()
			{
				return 1.0;
			}

			//------------------------------------------------------------------------------
			long OpenALSoundMixer::_GetUpdateThreadSleepTimeInNS() const
			{
				return 5 * 1000 * 1000;	// 5 ms
			}
		}
	}
}
