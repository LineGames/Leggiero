////////////////////////////////////////////////////////////////////////////////
// Common/ISoundPlayingHandle.h (Leggiero/Modules - Sound)
//
// Basic common control interface for playing sound
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__COMMON__I_SOUND_PLAYING_HANDLE_H
#define __LM_SOUND__COMMON__I_SOUND_PLAYING_HANDLE_H


// Leggiero.Sound
#include "SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		// Playing Sound Control Handle Interface
		class ISoundPlayingHandle
		{
		public:
			virtual bool IsFinished() const = 0;
			virtual void Stop() = 0;

			virtual bool IsPaused() const = 0;
			virtual void Pause() = 0;
			virtual void Resume() = 0;

			virtual SampleNumberType GetCurrentPosition() = 0;
			virtual bool Seek(SampleNumberType samplePosition) = 0;

			virtual float GetVolume() const = 0;
			virtual void SetVolume(float volume) = 0;
		};
	}
}

#endif
