////////////////////////////////////////////////////////////////////////////////
// Platform/iOSBGMPlayer.h (Leggiero/Modules - Sound)
//
// iOS Platform BGM Player using AVAudioPlayer
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PLATFORM__IOS_BGM_PLAYER_H
#define __LM_SOUND__PLATFORM__IOS_BGM_PLAYER_H


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
		namespace iOS
		{
			// Forward Declaration
			namespace _Internal
			{
				class iOSBGMPlayingContext;
			}


			// BGM Player Component
			class iOSBGMPlayer
				: public BGMPlayerComponent
			{
			public:
				iOSBGMPlayer(bool isUseBundleFileSystem = true);
				virtual ~iOSBGMPlayer();

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
				
				std::list<std::shared_ptr<_Internal::iOSBGMPlayingContext> >	m_contextHolders;
				Utility::Threading::SafePthreadLock								m_contextListLock;
				
			protected:
				void _UpdatePlay();
				static void *_UpdatePlayThreadHelper(void *threadThis);
			};

		}
	}
}

#endif
