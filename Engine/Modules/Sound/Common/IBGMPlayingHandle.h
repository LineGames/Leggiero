////////////////////////////////////////////////////////////////////////////////
// Common/IBGMPlayingHandle.h (Leggiero/Modules - Sound)
//
// Control Interface for Playing Music
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__COMMON__I_BGM_PLAYING_HANDLE_H
#define __LM_SOUND__COMMON__I_BGM_PLAYING_HANDLE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>

// Leggiero.Sound
#include "SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		// Playing BGM Control Handle Interface
		class IBGMPlayingHandle
		{
		public:
			virtual void Prepare() = 0;

			virtual void Play() = 0;
			virtual void Stop() = 0;
			virtual void Pause() = 0;

			virtual bool IsPlaying() = 0;

			virtual float GetLength() = 0;
			virtual float GetCurrentPosition() = 0;
			virtual bool Seek(float position) = 0;

			virtual bool IsLooping() = 0;
			virtual void SetLooping(bool isLooping) = 0;

			virtual float GetVolume() = 0;
			virtual void SetVolume(float volume) = 0;

		public:
			using PlayFinishHandlerType = std::function<void(IBGMPlayingHandle *, bool)>;
			virtual void RegisterOnPlayFinish(std::function<void(IBGMPlayingHandle *, bool)> handlerFunc) { m_playFinishHandler = handlerFunc; }

		protected:
			PlayFinishHandlerType m_playFinishHandler;

			void _OnPlayFinish(bool isSuccessful)
			{
				if (m_playFinishHandler)
				{
					m_playFinishHandler(this, isSuccessful);
				}
			}
		};
	}
}

#endif
