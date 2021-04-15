////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeSoundMixer.h (Leggiero/Modules - Sound)
//
// Sound Mixer in Oboe Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__OBOE_SOUND_MIXER_H
#define __LM_SOUND__OBOE_BACKEND__OBOE_SOUND_MIXER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <cstdint>
#include <list>
#include <memory>
#include <vector>
#include <unordered_map>

// External Library
#include <oboe/Oboe.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>
#include <Utility/Object/PointerHolder.h>
#include <Utility/Object/VerySimpleObjectPool.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Sound
#include "../SoundMixerComponent.h"
#include "OboeCommon.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Sound
	{
		namespace Oboe
		{
			// Forward Declarations
			class OboeSoundPlayingContext;
			class OboeBufferedSoundPlayingContext;
			class OboeStream;


			// Game Sound Mixer Component
			class OboeSoundMixer
				: public SoundMixerComponent
				, public Application::IApplicationEventObserver
				, public Application::ISystemEventObserver
			{
				friend class OboeSoundPlayingContext;
				friend class OboeBufferedSoundPlayingContext;
				friend class OboeStream;

			public:
				OboeSoundMixer();
				virtual ~OboeSoundMixer();

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

			public:	// Application::IApplicationEventObserver
				virtual void OnGoToBackground() override;
				virtual void OnReturnFromBackground() override;

			public:	// Application::ISystemEventObserver
				virtual void OnMemoryWarning() override;

			protected:
				std::atomic_bool m_isOnceInitialized;

				Application::ApplicationComponent *m_appComponentCopy;

				bool m_isSuspended;
				Utility::Threading::SafePthreadLock	m_suspendMutex;

				std::list<std::weak_ptr<SoundPlayingContext> >										m_playingSounds;
				std::unordered_map<SoundPlayingContext *, std::shared_ptr<SoundPlayingContext> >	m_playingSoundsHoldings;
				std::list<std::weak_ptr<BufferedSoundPlayingContext> >								m_playingBufferedSounds;
				std::unordered_map<BufferedSoundPlayingContext *, std::shared_ptr<BufferedSoundPlayingContext> >	m_playingBufferedSoundsHoldings;
				Utility::Threading::SafePthreadLock																	m_soundListMutex;

				bool		m_isShutdownRequested;
				pthread_t	m_soundUpdateThread;

				Utility::Threading::SafePthreadLock	m_cleanUpMutex;
				bool								m_isCleanUpDone;

				Utility::Threading::SafePthreadLock					m_updateLoopConsumeMutex;
				Utility::Threading::SafePthreadConditionVariable	m_updateLoopConsumeCondition;
				std::atomic<long long>								m_lastSignalTimeInEpochMicroSec;

				bool m_isUpdateThreadExists;

				Utility::Object::VerySimpleObjectPool<DataBufferType> m_segmentedBufferPool;
				Utility::Object::VerySimpleObjectPool<DataBufferType> m_fullBufferPool;

				std::shared_ptr<Utility::Object::PointerHolder> m_myPointerHolder;

				std::unordered_map<int32_t, OboeStream *>	m_streams;
				Utility::Threading::SafePthreadRWLock		m_streamsLock;

			protected:
				void _SuspendMixer();
				void _RestoreMixer();

				void _CleanUpContexts();

			protected:
				virtual bool _IsThreadPrioritySettingEnabled() const { return true; }
				void _StartSoundUpdateThread();

				long _GetUpdateThreadSleepTimeInNS() const;

				void _CheckAndSignalUpdate();

			protected:
				void _RegisterStreamSound(std::shared_ptr<IOboeStreamRendering> sound, int32_t samplingFrequency);
				void _InitializeStream(int32_t samplingFrequency, bool isInLock = true);

			protected:
				void _NotifyPlayingSoundResume(std::shared_ptr<SoundPlayingContext> context);
				void _NotifyPlayingBufferedSoundResume(std::shared_ptr<BufferedSoundPlayingContext> context);

			protected:
				void		_UpdatePlay();
				static void *_UpdatePlayThreadHelper(void *threadThis);
			};
		}
	}
}

#endif
