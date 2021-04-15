////////////////////////////////////////////////////////////////////////////////
// Platform/AndroidBGMPlayer.cpp (Leggiero/Modules - Sound)
//
// iOS Platform BGM Player Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidBGMPlayer.h"

// External Library
#include <errno.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ThreadSleep.h>

// Leggiero.Sound
#include "_AndroidBGMPlayingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// BGMPlayerComponent

		//------------------------------------------------------------------------------
		// BGM Player Component Factory Function
		BGMPlayerComponent *BGMPlayerComponent::CreateComponentObject(bool isUseBundleFileSystem)
		{
			return new Android::AndroidBGMPlayer();
		}


		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidBGMPlayer

			//------------------------------------------------------------------------------
			AndroidBGMPlayer::AndroidBGMPlayer()
				: m_isInitialized(false), m_isUpdateThreadExists(false)
			{
				
			}

			//------------------------------------------------------------------------------
			AndroidBGMPlayer::~AndroidBGMPlayer()
			{
				ShutdownComponent();
			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void AndroidBGMPlayer::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
				m_isShutdownRequested.store(false);

				if (pthread_create(&m_updateThread, NULL, _UpdatePlayThreadHelper, this) != 0)
				{
					// Update Thread Failed
					m_isUpdateThreadExists = false;
				}
				else
				{
					m_isUpdateThreadExists = true;
				}

				m_isInitialized.store(true);
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void AndroidBGMPlayer::ShutdownComponent()
			{
				bool shouldBeInitialized = true;
				if (!m_isInitialized.compare_exchange_strong(shouldBeInitialized, false))
				{
					// Not-Initialized
					return;
				}

				m_isShutdownRequested.store(true);
				if (m_isUpdateThreadExists)
				{
					pthread_join(m_updateThread, nullptr);
					m_isUpdateThreadExists = false;
				}
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IBGMPlayingHandle> AndroidBGMPlayer::PlayFromFile(const std::string &filePath, bool isStartImmediately, float volume, bool isLooping, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
			{
				if (!m_isInitialized.load())
				{
					return nullptr;
				}

				ContextIdType contextId = _IssueContextId();

				// Create Context
				std::shared_ptr<_Internal::AndroidBGMPlayingContext> bgmPlayingContext(_Internal::AndroidBGMPlayingContext::CreateContextWithFileResource(filePath, contextId));
				if (!bgmPlayingContext)
				{
					return nullptr;
				}

				// Set properties
				bgmPlayingContext->RegisterOnPlayFinish(playFinishHandler);
				bgmPlayingContext->SetVolume(volume);
				bgmPlayingContext->SetLooping(isLooping);

				// Register
				_RegisterContext(contextId, bgmPlayingContext);

				{
					auto lockContext = m_contextListLock.Lock();
					m_contextHolders.push_back(bgmPlayingContext);
				}

				if (isStartImmediately)
				{
					bgmPlayingContext->Play();
				}

				return bgmPlayingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IBGMPlayingHandle> AndroidBGMPlayer::PlayInBundle(const std::string &virtualPath, bool isStartImmediately, float volume, bool isLooping, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
			{
				if (!m_isInitialized.load())
				{
					return nullptr;
				}

				ContextIdType contextId = _IssueContextId();

				// Create Context
				std::shared_ptr<_Internal::AndroidBGMPlayingContext> bgmPlayingContext(_Internal::AndroidBGMPlayingContext::CreateContextWithBundleResource(virtualPath, contextId));
				if (!bgmPlayingContext)
				{
					return nullptr;
				}

				// Set properties
				bgmPlayingContext->RegisterOnPlayFinish(playFinishHandler);
				bgmPlayingContext->SetVolume(volume);
				bgmPlayingContext->SetLooping(isLooping);

				// Register
				_RegisterContext(contextId, bgmPlayingContext);

				{
					auto lockContext = m_contextListLock.Lock();
					m_contextHolders.push_back(bgmPlayingContext);
				}

				if (isStartImmediately)
				{
					bgmPlayingContext->Play();
				}

				return bgmPlayingContext;
			}
			
			//------------------------------------------------------------------------------
			// (Worker Thread) Update currently playing sounds
			void AndroidBGMPlayer::_UpdatePlay()
			{
				std::vector<std::weak_ptr<_Internal::AndroidBGMPlayingContext> > markedHolder;
				while (!m_isShutdownRequested.load())
				{
					// Check not playing contexts
					{
						auto lockContext = m_contextListLock.Lock();

						for (std::list<std::shared_ptr<_Internal::AndroidBGMPlayingContext> >::iterator it = m_contextHolders.begin(); it != m_contextHolders.end(); )
						{
							if ((*it)->IsPlaying())
							{
								++it;
							}
							else
							{
								markedHolder.push_back(*it);
								it = m_contextHolders.erase(it);
							}
						}
					}
					
					// Re-hold not collected items
					{
						auto lockContext = m_contextListLock.Lock();
						
						for (std::weak_ptr<_Internal::AndroidBGMPlayingContext> &currentMarked : markedHolder)
						{
							std::shared_ptr<_Internal::AndroidBGMPlayingContext> currentContext(currentMarked.lock());
							if (currentContext)
							{
								m_contextHolders.push_back(currentContext);
							}
						}
					}
					markedHolder.clear();

					_CollectIdContextGarbage();

					// This is a sort of GC without heavy usage
					Utility::Threading::PthreadSleepMS(900);
				}
			}

			//------------------------------------------------------------------------------
			// Sound Play Thread Helper Function
			void *AndroidBGMPlayer::_UpdatePlayThreadHelper(void *threadThis)
			{
				((AndroidBGMPlayer *)threadThis)->_UpdatePlay();
				return nullptr;
			}


			//////////////////////////////////////////////////////////////////////////////// AndroidBGMPlayer - Static Platform Support

			//------------------------------------------------------------------------------
			std::atomic<AndroidBGMPlayer::ContextIdType> AndroidBGMPlayer::ms_contextIdIssuer(1);

			Utility::Threading::SafePthreadRWLock AndroidBGMPlayer::ms_idContextTableLock;
			std::unordered_map<AndroidBGMPlayer::ContextIdType, std::weak_ptr<_Internal::AndroidBGMPlayingContext> > AndroidBGMPlayer::ms_idContextTable;

			//------------------------------------------------------------------------------
			std::shared_ptr<_Internal::AndroidBGMPlayingContext> AndroidBGMPlayer::_SearchPlayingContext(AndroidBGMPlayer::ContextIdType id)
			{
				std::weak_ptr<_Internal::AndroidBGMPlayingContext> foundContextPointer;

				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&ms_idContextTableLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *listLock = &ms_idContextTableLock.GetLock();
					auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					std::unordered_map<AndroidBGMPlayer::ContextIdType, std::weak_ptr<_Internal::AndroidBGMPlayingContext> >::iterator findIt = ms_idContextTable.find(id);
					if (findIt != ms_idContextTable.end())
					{
						foundContextPointer = findIt->second;
					}
				}

				return foundContextPointer.lock();
			}

			//------------------------------------------------------------------------------
			AndroidBGMPlayer::ContextIdType AndroidBGMPlayer::_IssueContextId()
			{
				return ms_contextIdIssuer.fetch_add(1);
			}

			//------------------------------------------------------------------------------
			void AndroidBGMPlayer::_RegisterContext(AndroidBGMPlayer::ContextIdType id, std::shared_ptr<_Internal::AndroidBGMPlayingContext> context)
			{
				int lockResult = pthread_rwlock_wrlock(&ms_idContextTableLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &ms_idContextTableLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					ms_idContextTable.insert(std::make_pair(id, context));
				}
				else
				{
					// Don't register without lock
				}
			}

			//------------------------------------------------------------------------------
			void AndroidBGMPlayer::_CollectIdContextGarbage()
			{
				static std::vector<AndroidBGMPlayer::ContextIdType> removedIds;

				int lockResult = pthread_rwlock_wrlock(&ms_idContextTableLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &ms_idContextTableLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					for (std::unordered_map<AndroidBGMPlayer::ContextIdType, std::weak_ptr<_Internal::AndroidBGMPlayingContext> >::iterator it = ms_idContextTable.begin(); it != ms_idContextTable.end(); ++it)
					{
						if (it->second.expired())
						{
							removedIds.push_back(it->first);
						}
					}

					for (AndroidBGMPlayer::ContextIdType currentIdToDelete : removedIds)
					{
						ms_idContextTable.erase(currentIdToDelete);
					}
					removedIds.clear();
				}
				else
				{
					// Don't do collection without lock
				}
			}
		}
	}
}
