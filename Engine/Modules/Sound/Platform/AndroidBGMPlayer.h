////////////////////////////////////////////////////////////////////////////////
// Platform/AndroidBGMPlayer.h (Leggiero/Modules - Sound)
//
// Adnroid Platform BGM Player using MediaPlayer
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PLATFORM__ANDROID_BGM_PLAYER_H
#define __LM_SOUND__PLATFORM__ANDROID_BGM_PLAYER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <list>
#include <unordered_map>

// External Library
#include <pthread.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>
#include <Utility/Object/PointerHolder.h>

// Leggiero.Sound
#include "../BGMPlayerComponent.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Android
		{
			// Forward Declaration
			namespace _Internal
			{
				class AndroidBGMPlayingContext;
			}


			// BGM Player Component
			class AndroidBGMPlayer
				: public BGMPlayerComponent
			{
			public:
				AndroidBGMPlayer();
				virtual ~AndroidBGMPlayer();

			public:	// EngineComponent
				// Initialize the Component
				virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

				// Safely Shutdown Component
				virtual void ShutdownComponent() override;

			public:	// BGMPlayerComponent
				virtual std::shared_ptr<IBGMPlayingHandle> PlayFromFile(const std::string &filePath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) override;
				virtual std::shared_ptr<IBGMPlayingHandle> PlayInBundle(const std::string &virtualPath, bool isStartImmediately = false, float volume = 1.0f, bool isLooping = false, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler = IBGMPlayingHandle::PlayFinishHandlerType()) override;

			protected:
				std::atomic_bool m_isInitialized;

				std::atomic_bool	m_isShutdownRequested;
				pthread_t			m_updateThread;
				bool				m_isUpdateThreadExists;

				std::list<std::shared_ptr<_Internal::AndroidBGMPlayingContext> >	m_contextHolders;
				Utility::Threading::SafePthreadLock									m_contextListLock;
				
			protected:
				void _UpdatePlay();
				static void *_UpdatePlayThreadHelper(void *threadThis);

			public:	// Android Platform Communication
				using ContextIdType = int32_t;

				static std::shared_ptr<_Internal::AndroidBGMPlayingContext> _SearchPlayingContext(ContextIdType id);

			protected:
				static std::atomic<ContextIdType> ms_contextIdIssuer;

				static Utility::Threading::SafePthreadRWLock													ms_idContextTableLock;
				static std::unordered_map<ContextIdType, std::weak_ptr<_Internal::AndroidBGMPlayingContext> >	ms_idContextTable;

				static ContextIdType _IssueContextId();

				static void _RegisterContext(ContextIdType id, std::shared_ptr<_Internal::AndroidBGMPlayingContext> context);
				static void _CollectIdContextGarbage();
			};
		}
	}
}

#endif
