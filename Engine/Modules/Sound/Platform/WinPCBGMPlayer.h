////////////////////////////////////////////////////////////////////////////////
// Platform/WinPCBGMPlayer.h (Leggiero/Modules - Sound)
//
// WinPC Platform BGM Player using Sound Mixer
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PLATFORM__WINPC_BGM_PLAYER_H
#define __LM_SOUND__PLATFORM__WINPC_BGM_PLAYER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <list>

// External Library
#include <pthread.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>
#include <Utility/Object/PointerHolder.h>

// Leggiero.Sound
#include "../BGMPlayerComponent.h"


namespace Leggiero
{
	// Forward Declaration
	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}


	namespace Sound
	{
		// Forward Declaration
		class SoundMixerComponent;


		namespace WinPC
		{
			// Forward Declaration
			namespace _Internal
			{
				class WinPCBGMPlayingContext;
			}


			// BGM Player Component
			class WinPCBGMPlayer
				: public BGMPlayerComponent
			{
			public:
				WinPCBGMPlayer(bool isUseBundleFileSystem = true);
				virtual ~WinPCBGMPlayer();

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

			public:	// BGMPlayerComponent
				virtual std::shared_ptr<IBGMPlayingHandle> PlayFromFile(const std::string &filePath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) override;
				virtual std::shared_ptr<IBGMPlayingHandle> PlayInBundle(const std::string &virtualPath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) override;

			protected:
				std::atomic_bool m_isInitialized;

				std::atomic_bool	m_isShutdownRequested;
				pthread_t			m_updateThread;
				bool				m_isUpdateThreadExists;

				FileSystem::BundleFileResourceComponent *m_pBundleFileSys;
				bool									m_isUseBundleFileSystem;
				SoundMixerComponent *m_pSoundMixer;

				std::list<std::weak_ptr<_Internal::WinPCBGMPlayingContext> >	m_processingContexts;
				std::list<std::shared_ptr<_Internal::WinPCBGMPlayingContext> >	m_contextHolders;
				Utility::Threading::SafePthreadLock								m_contextListLock;

			protected:
				enum class SupportedSoundFormatType
				{
					kINVALID, 
					kWAV, 
					kOGG,
					kMP3, 
				};

				static SupportedSoundFormatType _GetFormatFromPath(const std::string &path);

			protected:
				void _UpdatePlay();
				static void *_UpdatePlayThreadHelper(void *threadThis);
			};

		}
	}
}

#endif
