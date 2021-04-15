////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Base Sound Playing Context in OpenAL Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OPENAL_BACKEND__OPENAL_SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__OPENAL_BACKEND__OPENAL_SOUND_PLAYING_CONTEXT_H


// Standard Library
#include <atomic>
#include <queue>
#include <memory>
#include <vector>

// External Library
#include <OpenAL.h>

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>
#include <Utility/Sugar/NonCopyable.h>

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


			// OpenAL Sound Playing Context
			class OpenALSoundPlayingContext
				: public ISoundPlayingHandle
				, public std::enable_shared_from_this<OpenALSoundPlayingContext>
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				friend class OpenALSoundMixer;
				using MixerClass = OpenALSoundMixer;

			public:
				// Platform Dependent Settings
				static size_t g_bufferSize;

			public:
				virtual ~OpenALSoundPlayingContext();

			public:
				OpenALSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, ALuint source, ALuint buffers[], size_t bufferCount, bool isStartImmediately = true, float volume = 1.0f, bool isPauseAfterFinish = false);

			private:
				OpenALSoundPlayingContext() = delete;

			public:	// ISoundPlayingHandle
				virtual bool IsFinished() const override;
				virtual void Stop() override;

				virtual bool IsPaused() const override { return m_isPaused; }
				virtual void Pause() override;
				virtual void Resume() override;

				virtual SampleNumberType GetCurrentPosition() override { return GetLastQueuedSamplePosition(); }
				virtual bool Seek(SampleNumberType samplePosition) override;

				virtual float GetVolume() const override { return m_volume; }
				virtual void SetVolume(float volume) override;

			public:
				SampleNumberType GetLastQueuedSamplePosition() const { return m_lastSampleNumber; }
				SampleNumberType GetLastProcessedSamplePosition() const { return m_lastProcessedSamplePosition; }

				bool IsPlayUpdateStarted() const { return m_isPlayUpdateStarted; }
				bool IsSeekApplied() const { return m_isSeekApplied; }

				void SetWillPauseAfterFinish(bool isPauseAfterFinish) { m_isPauseAfterFinish = isPauseAfterFinish; }
				bool IsWillPauseAfterFinish() const { return m_isPauseAfterFinish; }

			protected:
				void _Update(std::vector<ALuint> &releasingBuffers);
				void _CancelBufferUsage();

			protected:
				std::shared_ptr<ISoundProvider> m_soundProvider;
				bool							m_isPlayFinished;
				bool							m_isAllDataQueued;
				SampleNumberType				m_lastSampleNumber;

				bool m_isPaused;
				bool m_isPauseAfterFinish;

				std::atomic_bool	m_isSourceReturned;
				ALuint				m_source;
				std::vector<ALuint> m_reservedBuffers;
				size_t				m_bufferCount;

				std::queue<SampleNumberType>	m_bufferSamples;
				SampleNumberType				m_lastProcessedSamplePosition;

				float	m_volume;
				bool	m_isVolumeChangeRequested;

				std::weak_ptr<Utility::Object::PointerHolder> m_mixerHolder;

			protected:
				bool m_isPlayUpdateStarted;
				bool m_isSeekApplied;
			};
		}
	}
}

#endif
