////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALBufferedSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// All data buffered Sound Playing Context in OpenAL Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OPENAL_BACKEND__OPENAL_BUFFERED_SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__OPENAL_BACKEND__OPENAL_BUFFERED_SOUND_PLAYING_CONTEXT_H


// Standard Library
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>

// External Library
#include <OpenAL.h>

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"
#include "../Common/ISoundPlayingHandle.h"


namespace Leggiero
{
	namespace Sound
	{
		// Forward Declaration
		class ISoundProvider;


		namespace OpenAL
		{
			// Forward Declaration
			class OpenALSoundMixer;


			// Buffered Sound Playing Context
			class OpenALBufferedSoundPlayingContext
				: public ISoundPlayingHandle
				, public std::enable_shared_from_this<OpenALBufferedSoundPlayingContext>
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				friend class OpenALSoundMixer;
				using MixerClass = OpenALSoundMixer;

			public:
				virtual ~OpenALBufferedSoundPlayingContext();

			public:
				OpenALBufferedSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, ALuint source, ALuint buffer, bool isStartImmediately = true, float volume = 1.0f, bool isLooping = false, bool isPauseAfterFinish = false);

			private:
				OpenALBufferedSoundPlayingContext() = delete;

			public:
				virtual bool IsFinished() const override;
				virtual void Stop() override;

				virtual bool IsPaused() const override;
				virtual void Pause() override;
				virtual void Resume() override;

				virtual SampleNumberType GetCurrentPosition() override;
				virtual bool Seek(SampleNumberType samplePosition) override;

				virtual float GetVolume() const override { return m_volume; }
				virtual void SetVolume(float volume) override;

			public:
				void SetIsLooping(bool isLooping);
				bool IsLooping();

				void Rewind();

				void SetWillPauseAfterFinish(bool isPauseAfterFinish) { m_isPauseAfterFinish = isPauseAfterFinish; }
				bool IsWillPauseAfterFinish() const { return m_isPauseAfterFinish; }

			protected:
				void _Update();

				void _PauseBySystem();
				void _ResumeBySystem();

			protected:
				void _FillBuffer(std::shared_ptr<ISoundProvider> &soundProvider);

			protected:
				bool	m_isPlayFinished;

				Utility::Threading::SafePthreadLock		m_pauseResumeCheckMutex;
				std::atomic_bool	m_isPaused;
				std::atomic_bool	m_isPausedBySystem;

				bool m_isPauseAfterFinish;

				std::atomic_bool	m_isSourceReturned;
				ALuint	m_source;
				ALuint	m_buffer;

				size_t	m_sampleSize;

				float	m_volume;
				bool	m_isVolumeChangeRequested;

				std::weak_ptr<Utility::Object::PointerHolder> m_mixerHolder;
			};

		}
	}
}

#endif
