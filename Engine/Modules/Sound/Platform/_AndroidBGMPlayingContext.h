////////////////////////////////////////////////////////////////////////////////
// Platform/_AndroidBGMPlayingContext.h (Leggiero/Modules - Sound)
//
// Internal Class for Adnroid Platform BGM Playing Context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PLATFORM___ANDROID_BGM_PLAYING_CONTEXT_H
#define __LM_SOUND__PLATFORM___ANDROID_BGM_PLAYING_CONTEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Platform.Android
#include <AndroidCommon.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "../BGMPlayerComponent.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Android
		{
			// Forward Declaration
			class AndroidBGMPlayer;


			namespace _Internal
			{
				// Android Platform C++ Playing Context
				class AndroidBGMPlayingContext
					: public IBGMPlayingHandle
				{
					friend class AndroidBGMPlayer;

				public:
					AndroidBGMPlayingContext(jobject javaPlayingContext);
					virtual ~AndroidBGMPlayingContext();

				public:	//IBGMPlayingHandle
					virtual void Prepare() override;

					virtual void Play() override;
					virtual void Stop() override;
					virtual void Pause() override;

					virtual bool IsPlaying() override;

					virtual float GetLength() override;
					virtual float GetCurrentPosition() override;
					virtual bool Seek(float position) override;

					virtual bool IsLooping() override;
					virtual void SetLooping(bool isLooping) override;

					virtual float GetVolume() override;
					virtual void SetVolume(float volume) override;

				public:
					void NotifyFinishPlayingEvent(bool successfully);

				public:
					static std::shared_ptr<AndroidBGMPlayingContext> CreateContextWithFileResource(const std::string &filePath, int contextId);
					static std::shared_ptr<AndroidBGMPlayingContext> CreateContextWithBundleResource(const std::string &virtualPath, int contextId);

				protected:
					Utility::Threading::SafePthreadLock	m_contextLock;
					jobject m_javaPlayingContext;
					jclass m_javaContextClass;

					float m_lastVolume;
				};
			}
		}
	}
}

#endif
