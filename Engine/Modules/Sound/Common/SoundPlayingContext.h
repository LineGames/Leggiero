////////////////////////////////////////////////////////////////////////////////
// Common/SoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Definition of Real Sound Playing Context by Platforms
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__COMMON__SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__COMMON__SOUND_PLAYING_CONTEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


// Include Backend Playing Context by Platform
#if defined(_LEGGIERO_WINPC) || defined(_LEGGIERO_IOS)

	// OpenAL Backend
	#include "../OpenALBackend/OpenALSoundPlayingContext.h"
	#include "../OpenALBackend/OpenALLoopingSoundPlayingContext.h"
	#include "../OpenALBackend/OpenALBufferedSoundPlayingContext.h"

	namespace Leggiero
	{
		namespace Sound
		{
			using SoundPlayingContext = OpenAL::OpenALSoundPlayingContext;
			using LoopingSoundPlayingContext = OpenAL::OpenALLoopingSoundPlayingContext;
			using BufferedSoundPlayingContext = OpenAL::OpenALBufferedSoundPlayingContext;
		}
	}

#elif defined(_LEGGIERO_ANDROID)

	// Oboe Backend
	#include "../OboeBackend/OboeSoundPlayingContext.h"
	#include "../OboeBackend/OboeLoopingSoundPlayingContext.h"
	#include "../OboeBackend/OboeBufferedSoundPlayingContext.h"

	namespace Leggiero
	{
		namespace Sound
		{
			using SoundPlayingContext = Oboe::OboeSoundPlayingContext;
			using LoopingSoundPlayingContext = Oboe::OboeLoopingSoundPlayingContext;
			using BufferedSoundPlayingContext = Oboe::OboeBufferedSoundPlayingContext;
		}
	}

#endif


#endif
