////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALSoundMixer.h (Leggiero/Modules - Sound)
//
// Sound Mixer in OpenAL Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OPENAL_BACKEND__OPENAL_SOUND_MIXER_H
#define __LM_SOUND__OPENAL_BACKEND__OPENAL_SOUND_MIXER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <list>
#include <memory>
#include <vector>
#include <unordered_map>

// External Library
#include <OpenAL.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>
#include <Utility/Object/PointerHolder.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Sound
#include "../SoundMixerComponent.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Sound
	{
		namespace OpenAL
		{
			// Forward Declaration
			class OpenALSoundPlayingContext;
			class OpenALBufferedSoundPlayingContext;
			
			namespace _Internal
			{
				class OpenALSourceReleasingContext;
			}


			// Game Sound Mixer Component
			class OpenALSoundMixer
				: public SoundMixerComponent
				, public Application::IApplicationEventObserver
			{
				friend class OpenALSoundPlayingContext;
				friend class OpenALBufferedSoundPlayingContext;

			public:
				OpenALSoundMixer();
				virtual ~OpenALSoundMixer();

			public:	// EngineComponent
				// Initialize the Component
				virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

				// Safely Shutdown Component
				virtual void ShutdownComponent() override;

				// Get type Id list of dependent components needed by this component
				virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const override;

				// Inject Dependency to the Component.
				// All dependency injections will be done before the initialization.
				virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) override;

			public:	// SoundMixerComponent
				virtual std::shared_ptr<SoundPlayingContext> Play(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately = true, float volume = 1.0f, bool isPauseAfterFinish = false) override;
				virtual std::shared_ptr<LoopingSoundPlayingContext> PlayLoopingSound(std::shared_ptr<ISoundProvider> sound, SampleNumberType loopStartSamplePosition, SampleNumberType loopFinishSamplePosition, bool isStartImmediately = true, float volume = 1.0f) override;
				virtual std::shared_ptr<BufferedSoundPlayingContext> PlayBufferedSound(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately = true, float volume = 1.0f, bool isLooping = false, bool isPauseAfterFinish = false) override;

				virtual void ClearNonManagedSounds() override;

				virtual bool IsSuspended() const override { return m_isSuspended; }
				virtual void HintUpdate() override;

			public:
				static size_t GetDefaultBufferForSoundCount();
				static double GetMixerExpectedPlayDelayRatio();

			protected:
				ALCdevice *m_device;
				ALCcontext *m_context;

				std::atomic_bool m_isOnceInitialized;

				Application::ApplicationComponent *m_appComponentCopy;

				bool            m_isSuspended;
				Utility::Threading::SafePthreadLock	m_suspendMutex;

				std::list<std::weak_ptr<SoundPlayingContext> >		m_playingSounds;
				std::unordered_map<SoundPlayingContext *, std::shared_ptr<SoundPlayingContext> >	m_playingSoundsHoldings;
				std::list<std::weak_ptr<BufferedSoundPlayingContext> >	m_playingBufferedSounds;
				std::unordered_map<BufferedSoundPlayingContext *, std::shared_ptr<BufferedSoundPlayingContext> >	m_playingBufferedSoundsHoldings;
				std::list<std::shared_ptr<_Internal::OpenALSourceReleasingContext> > m_releasingContexts;
				Utility::Threading::SafePthreadLock						m_soundListMutex;

				bool		m_isShutdownRequested;
				pthread_t	m_soundUpdateThread;

				Utility::Threading::SafePthreadLock	m_cleanUpMutex;
				bool								m_isCleanUpDone;

				Utility::Threading::SafePthreadLock					m_updateLoopConsumeMutex;
				Utility::Threading::SafePthreadConditionVariable	m_updateLoopConsumeCondition;
				std::atomic<long long>								m_lastSignalTimeInEpochMicroSec;

				bool m_isUpdateThreadExists;

				std::list<std::weak_ptr<BufferedSoundPlayingContext> >	m_suspendPausedBufferedSounds;

				std::shared_ptr<Utility::Object::PointerHolder> m_myPointerHolder;

			protected:
				bool _StartOpenAL();
				void _SuspendOpenAL();
				void _RestoreOpenAL();
				void _FinishOpenAL();

				void _CleanUpContexts();

			protected:
				bool	_RetainFreeSource(ALuint *outSource);
				void	_ReleaseSource(ALuint source);
				void	_ReleaseSources(std::vector<ALuint> &sources);

				size_t	_RetainFreeBuffer(ALuint *outBuffers, size_t count);
				void	_ReleaseBuffers(ALuint buffers[], size_t count);

			protected:
				virtual bool _IsThreadPrioritySettingEnabled() const { return true; }
				void _StartSoundUpdateThread();

				long _GetUpdateThreadSleepTimeInNS() const;

				void _CheckAndSignalUpdate();

			protected:
				void _NotifyPlayingSoundResume(std::shared_ptr<SoundPlayingContext> context);
				void _NotifyPlayingBufferedSoundResume(std::shared_ptr<BufferedSoundPlayingContext> context);

				void _PushSourceToRelease(ALuint source);

			protected:
				virtual void _InitializeOptiomalGlobalSoundBufferSize() { /* Use Initial Setted Value */ }

			protected:
				void		_UpdatePlay();
				static void *_UpdatePlayThreadHelper(void *threadThis);
			};

		}
	}
}

#endif
