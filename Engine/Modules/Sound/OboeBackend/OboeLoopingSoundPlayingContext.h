////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeLoopingSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Looping Playing Context in Oboe Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__LOOPING_SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__OBOE_BACKEND__LOOPING_SOUND_PLAYING_CONTEXT_H


// Leggiero.Sound
#include "OboeSoundPlayingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			// Looping Sound Playing Context
			class OboeLoopingSoundPlayingContext
				: public OboeSoundPlayingContext
			{
				friend class OboeSoundMixer;

			public:
				virtual ~OboeLoopingSoundPlayingContext() { }

			public:
				OboeLoopingSoundPlayingContext(std::shared_ptr<ISoundProvider> sourceSoundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder,
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
