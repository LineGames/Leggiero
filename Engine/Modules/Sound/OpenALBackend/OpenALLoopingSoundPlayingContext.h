////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALLoopingSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Looping Playing Context in OpenAL Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OPENAL_BACKEND__LOOPING_SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__OPENAL_BACKEND__LOOPING_SOUND_PLAYING_CONTEXT_H


// Leggiero.Sound
#include "OpenALSoundPlayingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			// Looping Sound Playing Context
			class OpenALLoopingSoundPlayingContext
				: public OpenALSoundPlayingContext
			{
				friend class OpenALSoundMixer;

			public:
				virtual ~OpenALLoopingSoundPlayingContext() { }

			public:
				OpenALLoopingSoundPlayingContext(std::shared_ptr<ISoundProvider> sourceSoundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, ALuint source, ALuint buffers[], size_t bufferCount,
					SampleNumberType loopSectionStart, SampleNumberType loopSectionFinish,
					bool isStartImmediately = true, float volume = 1.0f);

			public:
				void StopLooping();

			protected:
				bool m_isLooping;
			};
		}
	}
}

#endif
