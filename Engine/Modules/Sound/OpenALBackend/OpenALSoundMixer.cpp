////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALSoundMixer.cpp (Leggiero/Modules - Sound)
//
// Sound Mixer Implementation in OpenAL Backend
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OpenALSoundMixer.h"

// Standard Library
#include <chrono>
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ThreadSleep.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventDispatcher.h>

// Leggiero.Sound
#include "OpenALCommon.h"
#include "_Internal/_OpenALSourceReleasingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			//////////////////////////////////////////////////////////////////////////////// Code-Level Settings

			namespace _Internal
			{
				constexpr size_t kSoundDefaultBufferCountPerSound = 3;
			}


			//////////////////////////////////////////////////////////////////////////////// OpenALSoundMixer

			//------------------------------------------------------------------------------
			OpenALSoundMixer::OpenALSoundMixer()
				: m_isOnceInitialized(false), m_appComponentCopy(nullptr)
				, m_isSuspended(true), m_suspendMutex(true, false), m_soundListMutex(true, false), m_isShutdownRequested(false), m_isUpdateThreadExists(false), m_cleanUpMutex(true, false), m_isCleanUpDone(false)
				, m_updateLoopConsumeMutex(true, false), m_lastSignalTimeInEpochMicroSec((long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
				, m_myPointerHolder(std::make_shared<Utility::Object::PointerHolder>(this))
			{
				
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_StartSoundUpdateThread()
			{
				// Sound Thread Setting
				pthread_attr_t *pEffectiveSoundUpdateThreadAttribute = nullptr;
				pthread_attr_t soundUpdateThreadAttribute;
				pthread_attr_t *pSoundThreadAttrToRelease = nullptr;
				if (pthread_attr_init(&soundUpdateThreadAttribute) == 0)
				{
					pSoundThreadAttrToRelease = &soundUpdateThreadAttribute;
					pEffectiveSoundUpdateThreadAttribute = &soundUpdateThreadAttribute;
				}
				{
					auto soundThreadAttrReleaseFunc = [&pSoundThreadAttrToRelease]() mutable { if (pSoundThreadAttrToRelease != nullptr) pthread_attr_destroy(pSoundThreadAttrToRelease); };
					FINALLY_OF_BLOCK(_RELEASE_SOUND_THREAD_ATTR, soundThreadAttrReleaseFunc);

					sched_param soundThreadSchedParam;

					if (pEffectiveSoundUpdateThreadAttribute != nullptr && _IsThreadPrioritySettingEnabled())
					{
						int soundThreadSchedulingPolicy = SCHED_RR;
						bool isPolicySettingSuccess = true;
						if (pthread_attr_setschedpolicy(pEffectiveSoundUpdateThreadAttribute, soundThreadSchedulingPolicy) != 0)
						{
							// Policy Error - try FIFO
							soundThreadSchedulingPolicy = SCHED_FIFO;
							if (pthread_attr_setschedpolicy(pEffectiveSoundUpdateThreadAttribute, soundThreadSchedulingPolicy) != 0)
							{
								isPolicySettingSuccess = false;
							}
						}
						bool isPrioritySettingSuccess = true;
						if (isPolicySettingSuccess)
						{
							int soundThreadMinPriority = sched_get_priority_min(soundThreadSchedulingPolicy);
							int soundThreadMaxPriority = sched_get_priority_max(soundThreadSchedulingPolicy);
							if (soundThreadMinPriority == -1 || soundThreadMaxPriority == -1)
							{
								isPrioritySettingSuccess = false;
							}
							else
							{
								int soundThreadTargetPriority = soundThreadMaxPriority;
								if (soundThreadMinPriority < soundThreadMaxPriority - 1)
								{
									--soundThreadTargetPriority;
								}

								soundThreadSchedParam.sched_priority = soundThreadTargetPriority;
								if (pthread_attr_setschedparam(pEffectiveSoundUpdateThreadAttribute, &soundThreadSchedParam) != 0)
								{
									isPrioritySettingSuccess = false;
								}
							}
						}

						if (!isPolicySettingSuccess || !isPrioritySettingSuccess)
						{
							// Just Create a Default Thread
							pEffectiveSoundUpdateThreadAttribute = nullptr;
						}
					}
					else
					{
						pEffectiveSoundUpdateThreadAttribute = nullptr;
					}
					if (pthread_create(&m_soundUpdateThread, pEffectiveSoundUpdateThreadAttribute, _UpdatePlayThreadHelper, this) != 0)
					{
						// Update Thread Failed
						m_isUpdateThreadExists = false;

						if (pthread_create(&m_soundUpdateThread, NULL, _UpdatePlayThreadHelper, this) != 0)
						{
							// Update Thread Finally Failed
							m_isUpdateThreadExists = false;

							// FAILOVER PROCESS IMPLEMENTATION NEEDED
						}
						else
						{
							// Success with fallback
							m_isUpdateThreadExists = true;
						}
					}
					else
					{
						m_isUpdateThreadExists = true;
					}
				}
			}

			//------------------------------------------------------------------------------
			OpenALSoundMixer::~OpenALSoundMixer()
			{
				m_myPointerHolder->NotifyTargetDeleted();
				ShutdownComponent();
			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void OpenALSoundMixer::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
				if (!_StartOpenAL())
				{
					// OpenGL Initialization Failed
					return;
				}

				_StartSoundUpdateThread();

				_InitializeOptiomalGlobalSoundBufferSize();

				if (m_appComponentCopy != nullptr)
				{
					m_appComponentCopy->SystemEventCenter().RegisterApplicationEventObserver(this);
				}

				m_isOnceInitialized.store(true);
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void OpenALSoundMixer::ShutdownComponent()
			{
				bool isInitialized = true;
				if (!m_isOnceInitialized.compare_exchange_strong(isInitialized, false))
				{
					// Only one shutdown for a starting
					return;
				}

				if (m_appComponentCopy != nullptr)
				{
					m_appComponentCopy->SystemEventCenter().UnRegisterApplicationEventObserver(this);
				}

				m_isShutdownRequested = true;
				if (m_isUpdateThreadExists)
				{
					pthread_cond_signal(&m_updateLoopConsumeCondition.GetConditionVariable());
					pthread_join(m_soundUpdateThread, nullptr);
					m_isUpdateThreadExists = false;
				}
				else
				{
					_CleanUpContexts();
				}

				_FinishOpenAL();
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent components needed by this component
			const std::vector<EngineComponentIdType> &OpenALSoundMixer::GetDependentComponents() const
			{
				static std::vector<EngineComponentIdType> dependentComponentList{
					EngineComponentIdType::kApplication
				};
				return dependentComponentList;
			}

			//------------------------------------------------------------------------------
			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			void OpenALSoundMixer::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
			{
				switch (componentId)
				{
					case EngineComponentIdType::kApplication:
						{
							m_appComponentCopy = dynamic_cast<Application::ApplicationComponent *>(dependentComponent);
						}
						break;
				}
			}

			//------------------------------------------------------------------------------
			bool OpenALSoundMixer::_StartOpenAL()
			{
				m_device = alcOpenDevice(NULL);
				if (m_device == nullptr)
				{
					// Cannot open device
					return false;
				}

				m_context = alcCreateContext(m_device, NULL);
				if (m_context == nullptr)
				{
					// Cannot create context
					return false;
				}
				if (alcMakeContextCurrent(m_context) == ALC_FALSE)
				{
					// Cannot make context current
					return false;
				}

				m_isSuspended = false;

				return true;
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_SuspendOpenAL()
			{
				int lockResult = pthread_mutex_lock(&m_suspendMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_suspendMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_isSuspended = true;

					// Stop Buffered Musics
					for (std::list<std::weak_ptr<BufferedSoundPlayingContext> >::iterator it = m_playingBufferedSounds.begin(); it != m_playingBufferedSounds.end(); ++it)
					{
						std::shared_ptr<BufferedSoundPlayingContext> sharedPlayingContext(it->lock());
						if (sharedPlayingContext)
						{
							if (!sharedPlayingContext->IsFinished())
							{
								sharedPlayingContext->_PauseBySystem();
								m_suspendPausedBufferedSounds.push_back(sharedPlayingContext);
							}
						}
					}
				}
				else
				{
					// Anyway, Go
					m_isSuspended = true;

					// Stop Buffered Musics
					for (std::list<std::weak_ptr<BufferedSoundPlayingContext> >::iterator it = m_playingBufferedSounds.begin(); it != m_playingBufferedSounds.end(); ++it)
					{
						std::shared_ptr<BufferedSoundPlayingContext> sharedPlayingContext(it->lock());
						if (sharedPlayingContext)
						{
							if (!sharedPlayingContext->IsFinished())
							{
								sharedPlayingContext->_PauseBySystem();
								m_suspendPausedBufferedSounds.push_back(sharedPlayingContext);
							}
						}
					}
				}

				ALCcontext *contextToSuspend = alcGetCurrentContext();
				if (contextToSuspend == nullptr || contextToSuspend != m_context)
				{
					// Our context is NOT current
					return;
				}
				alcMakeContextCurrent(NULL);
				alcSuspendContext(contextToSuspend);

				pthread_cond_signal(&m_updateLoopConsumeCondition.GetConditionVariable());
				pthread_join(m_soundUpdateThread, nullptr);
				m_isUpdateThreadExists = false;
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_RestoreOpenAL()
			{
				if (m_context == nullptr)
				{
					// Invalid Context
					return;
				}
				if (alcMakeContextCurrent(m_context) == ALC_FALSE)
				{
					// Cannot make context current
					return;
				}
				alcProcessContext(m_context);

				if (m_isUpdateThreadExists)
				{
					// Wait for Gradual Hand-over
					Utility::Threading::PthreadSleep(_GetUpdateThreadSleepTimeInNS());
				}

				int lockResult = pthread_mutex_lock(&m_suspendMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_suspendMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_isSuspended = false;
				}
				else
				{
					// Anyway, Go
					m_isSuspended = false;
				}

				_StartSoundUpdateThread();
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_FinishOpenAL()
			{
				m_isSuspended = true;

				if (m_context != nullptr)
				{
					ALCcontext *contextToFinish = alcGetCurrentContext();
					if (contextToFinish == m_context)
					{
						alcMakeContextCurrent(NULL);
					}
					alcDestroyContext(m_context);
					m_context = nullptr;
				}
				if (m_device != nullptr)
				{
					alcCloseDevice(m_device);
					m_device = nullptr;
				}
			}

			//------------------------------------------------------------------------------
			bool OpenALSoundMixer::_RetainFreeSource(ALuint *outSource)
			{
				ALuint createdSource;
				alGenSources(1, &createdSource);
				if (alGetError() != AL_NO_ERROR)
				{
					// New Source Create Failed
					return false;
				}

				*outSource = createdSource;
				return true;
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_ReleaseSource(ALuint source)
			{
				alDeleteSources(1, &source);
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_ReleaseSources(std::vector<ALuint> &sources)
			{
				if (sources.empty())
				{
					return;
				}

				alDeleteSources(static_cast<ALsizei>(sources.size()), &sources[0]);
			}

			//------------------------------------------------------------------------------
			size_t OpenALSoundMixer::_RetainFreeBuffer(ALuint *outBuffers, size_t count)
			{
				ALuint *createdBuffer = (ALuint *)malloc(sizeof(ALuint) * count);
				if (createdBuffer == nullptr)
				{
					return 0;
				}
				{
					auto freeBufferArrayFunc = [createdBuffer]() mutable { free(createdBuffer); };
					FINALLY_OF_BLOCK(_FREE_BUFFER_ARRAY, freeBufferArrayFunc);

					alGenBuffers((ALsizei)count, createdBuffer);
					if (alGetError() != AL_NO_ERROR)
					{
						// New Buffer Create Failed
						return 0;
					}

					for (int i = 0; i < count; ++i)
					{
						outBuffers[i] = createdBuffer[i];
					}

					return count;
				}
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_ReleaseBuffers(ALuint buffers[], size_t count)
			{
				alDeleteBuffers(static_cast<ALsizei>(count), &buffers[0]);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<SoundPlayingContext> OpenALSoundMixer::Play(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately, float volume, bool isPauseAfterFinish)
			{
				if (sound == nullptr)
				{
					return nullptr;
				}

				ALuint playSource;
				if (!_RetainFreeSource(&playSource))
				{
					// Source Not Available
					return nullptr;
				}
				
				ALuint playBuffers[_Internal::kSoundDefaultBufferCountPerSound];
				size_t playBufferCount = _RetainFreeBuffer(playBuffers, _Internal::kSoundDefaultBufferCountPerSound);
				if (playBufferCount == 0)
				{
					// Buffer Not Available
					_ReleaseSource(playSource);
					return nullptr;
				}

				std::shared_ptr<SoundPlayingContext> playingContext = std::make_shared<SoundPlayingContext>(sound, m_myPointerHolder, playSource, playBuffers, playBufferCount, isStartImmediately, volume, isPauseAfterFinish);
				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_soundListMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_playingSoundsHoldings.insert(std::make_pair(playingContext.get(), playingContext));
					m_playingSounds.push_back(playingContext);
				}
				else
				{
					// Lock Failed
					_ReleaseSource(playSource);
					playingContext->_CancelBufferUsage();
					_ReleaseBuffers(playBuffers, playBufferCount);
					return nullptr;
				}

				_CheckAndSignalUpdate();

				return playingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<LoopingSoundPlayingContext> OpenALSoundMixer::PlayLoopingSound(std::shared_ptr<ISoundProvider> sound, SampleNumberType loopStartSamplePosition, SampleNumberType loopFinishSamplePosition, bool isStartImmediately, float volume)
			{
				if (sound == nullptr)
				{
					return nullptr;
				}

				ALuint playSource;
				if (!_RetainFreeSource(&playSource))
				{
					// Source Not Available
					return nullptr;
				}

				ALuint playBuffers[_Internal::kSoundDefaultBufferCountPerSound];
				size_t playBufferCount = _RetainFreeBuffer(playBuffers, _Internal::kSoundDefaultBufferCountPerSound);
				if (playBufferCount == 0)
				{
					// Buffer Not Available
					_ReleaseSource(playSource);
					return nullptr;
				}

				std::shared_ptr<LoopingSoundPlayingContext> playingContext = std::make_shared<LoopingSoundPlayingContext>(sound, m_myPointerHolder, playSource, playBuffers, playBufferCount, loopStartSamplePosition, loopFinishSamplePosition, isStartImmediately, volume);
				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_soundListMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_playingSoundsHoldings.insert(std::make_pair((SoundPlayingContext *)playingContext.get(), playingContext));
					m_playingSounds.push_back(playingContext);
				}
				else
				{
					// Lock Failed
					_ReleaseSource(playSource);
					playingContext->_CancelBufferUsage();
					_ReleaseBuffers(playBuffers, playBufferCount);
					return nullptr;
				}

				_CheckAndSignalUpdate();

				return playingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<BufferedSoundPlayingContext> OpenALSoundMixer::PlayBufferedSound(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately, float volume, bool isLooping, bool isPauseAfterFinish)
			{
				if (sound == nullptr)
				{
					return nullptr;
				}

				ALuint playSource;
				if (!_RetainFreeSource(&playSource))
				{
					// Source Not Available
					return nullptr;
				}

				ALuint playBuffer;
				size_t playBufferCount = _RetainFreeBuffer(&playBuffer, 1);
				if (playBufferCount == 0)
				{
					// Buffer Not Available
					_ReleaseSource(playSource);
					return nullptr;
				}

				std::shared_ptr<BufferedSoundPlayingContext> playingContext = std::make_shared<BufferedSoundPlayingContext>(sound, m_myPointerHolder, playSource, playBuffer, isStartImmediately, volume, isLooping, isPauseAfterFinish);
				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_soundListMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_playingBufferedSoundsHoldings.insert(std::make_pair(playingContext.get(), playingContext));
					m_playingBufferedSounds.push_back(playingContext);
				}
				else
				{
					// Lock Failed
					return nullptr;
				}

				_CheckAndSignalUpdate();

				return playingContext;
			}

			//------------------------------------------------------------------------------
			// Clear all non-managed(user having handle) sounds
			void OpenALSoundMixer::ClearNonManagedSounds()
			{
				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_soundListMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					// Clear Holdings
					m_playingSoundsHoldings.clear();
					m_playingBufferedSoundsHoldings.clear();

					// Check and re-process alived contexts
					for (std::list<std::weak_ptr<SoundPlayingContext> >::iterator it = m_playingSounds.begin(); it != m_playingSounds.end(); )
					{
						std::shared_ptr<SoundPlayingContext> playingContext(it->lock());
						if (playingContext)
						{
							SoundPlayingContext *contextRawPointer = playingContext.get();
							if (contextRawPointer->IsFinished())
							{
								it = m_playingSounds.erase(it);
							}
							else
							{
								if (!contextRawPointer->IsPaused())
								{
									m_playingSoundsHoldings.insert(std::make_pair(contextRawPointer, playingContext));
								}
								++it;
							}
						}
						else
						{
							it = m_playingSounds.erase(it);
						}
					}
					for (std::list<std::weak_ptr<BufferedSoundPlayingContext> >::iterator it = m_playingBufferedSounds.begin(); it != m_playingBufferedSounds.end(); )
					{
						std::shared_ptr<BufferedSoundPlayingContext> sharedPlayingContext(it->lock());
						if (sharedPlayingContext)
						{
							BufferedSoundPlayingContext *contextRawPointer = sharedPlayingContext.get();
							if (contextRawPointer->IsFinished())
							{
								it = m_playingBufferedSounds.erase(it);
							}
							else
							{
								if (!contextRawPointer->IsPaused())
								{
									m_playingBufferedSoundsHoldings.insert(std::make_pair(contextRawPointer, sharedPlayingContext));
								}
								++it;
							}
						}
						else
						{
							it = m_playingBufferedSounds.erase(it);
						}
					}
				}
				else
				{
					// Do nothing without lock
				}
			}

			//------------------------------------------------------------------------------
			// Hint immediate sound update to fast response
			void OpenALSoundMixer::HintUpdate()
			{
				m_lastSignalTimeInEpochMicroSec.store((long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
				pthread_cond_signal(&m_updateLoopConsumeCondition.GetConditionVariable());
			}

			//------------------------------------------------------------------------------
			// (Sound Play Thread) Update currently playing sounds
			void OpenALSoundMixer::_UpdatePlay()
			{
				pthread_t currentThread = pthread_self();
				bool isFirstCheckedValid = false;

				long loopSleepLimitTimeInNS = _GetUpdateThreadSleepTimeInNS();
				timespec loopSleepLimitTime;
				loopSleepLimitTime.tv_sec = 0;
				loopSleepLimitTime.tv_nsec = loopSleepLimitTimeInNS;

				long loopSleepMinimumTimeInNS = 1000000;	// 1ms
				if (loopSleepMinimumTimeInNS > loopSleepLimitTimeInNS)
				{
					loopSleepMinimumTimeInNS = loopSleepLimitTimeInNS;
				}

				timespec currentWaitTimeBuffer;

				std::chrono::steady_clock::time_point beforeThreadCalculation;
				std::chrono::steady_clock::duration threadCalculationDuration;

				std::vector<ALuint> releasedBuffers;
				std::vector<ALuint> releasedSources;
				std::vector<std::shared_ptr<SoundPlayingContext> > loopContextLifeHoldings;
				std::vector<std::shared_ptr<BufferedSoundPlayingContext> > loopBufferedContextLifeHoldings;
				while (!m_isShutdownRequested)
				{
					beforeThreadCalculation = std::chrono::steady_clock::now();

					int lockResult = pthread_mutex_lock(&m_suspendMutex.GetLock());
					if (lockResult == 0)
					{
						pthread_mutex_t *suspendLockCopy = &m_suspendMutex.GetLock();
						auto releaseSuspendLockFunc = [suspendLockCopy]() mutable { pthread_mutex_unlock(suspendLockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_SUSPEND_LOCK, releaseSuspendLockFunc);

						if (pthread_equal(currentThread, m_soundUpdateThread) == 0)
						{
							if (isFirstCheckedValid)
							{
								// New Thread Created
								return;
							}
						}
						else if (!isFirstCheckedValid)
						{
							isFirstCheckedValid = true;
						}

						if (m_isSuspended)
						{
							// Stop Update Thread
							return;
						}
						else
						{
							// Resume Buffered Musics
							if (!m_suspendPausedBufferedSounds.empty())
							{
								// Need to check logical pause flag
								for (std::list<std::weak_ptr<BufferedSoundPlayingContext> >::iterator it = m_suspendPausedBufferedSounds.begin(); it != m_suspendPausedBufferedSounds.end(); ++it)
								{
									std::shared_ptr<BufferedSoundPlayingContext> sharedPlayingContext(it->lock());
									if (sharedPlayingContext)
									{
										sharedPlayingContext->_ResumeBySystem();
									}
								}
								m_suspendPausedBufferedSounds.clear();
							}

							// IMPORTANT!! Lock Order: 1. m_suspendMutex - 2. m_soundListMutex
							lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
							if (lockResult == 0)
							{
								pthread_mutex_t *soundListLockCopy = &m_soundListMutex.GetLock();
								auto releaseSoundListLockFunc = [soundListLockCopy]() mutable { pthread_mutex_unlock(soundListLockCopy); };
								FINALLY_OF_BLOCK(_RELEASE_SOUNDLIST_LOCK, releaseSoundListLockFunc);

								for (std::list<std::weak_ptr<SoundPlayingContext> >::iterator it = m_playingSounds.begin(); it != m_playingSounds.end(); )
								{
									std::shared_ptr<SoundPlayingContext> playingContext(it->lock());
									if (playingContext)
									{
										SoundPlayingContext *contextRawPointer = playingContext.get();
										contextRawPointer->_Update(releasedBuffers);
										if (contextRawPointer->IsFinished())
										{
											bool isSourceNotReturned = false;
											if (contextRawPointer->m_isSourceReturned.compare_exchange_strong(isSourceNotReturned, true))
											{
												releasedSources.push_back(contextRawPointer->m_source);
											}
											it = m_playingSounds.erase(it);
											if (m_playingSoundsHoldings.find(contextRawPointer) != m_playingSoundsHoldings.end())
											{
												loopContextLifeHoldings.push_back(playingContext);
												m_playingSoundsHoldings.erase(contextRawPointer);
											}
										}
										else
										{
											if (contextRawPointer->IsPaused())
											{
												if (m_playingSoundsHoldings.find(contextRawPointer) != m_playingSoundsHoldings.end())
												{
													loopContextLifeHoldings.push_back(playingContext);
													m_playingSoundsHoldings.erase(contextRawPointer);
												}
											}
											else
											{
												if (m_playingSoundsHoldings.find(contextRawPointer) == m_playingSoundsHoldings.end())
												{
													m_playingSoundsHoldings.insert(std::make_pair(contextRawPointer, playingContext));
												}
											}
											++it;
										}
									}
									else
									{
										it = m_playingSounds.erase(it);
									}
								}

								for (std::list<std::weak_ptr<BufferedSoundPlayingContext> >::iterator it = m_playingBufferedSounds.begin(); it != m_playingBufferedSounds.end(); )
								{
									std::shared_ptr<BufferedSoundPlayingContext> sharedPlayingContext(it->lock());
									if (sharedPlayingContext)
									{
										BufferedSoundPlayingContext *contextRawPointer = sharedPlayingContext.get();
										contextRawPointer->_Update();
										if (contextRawPointer->IsFinished())
										{
											bool isBufferedSourceNotReturned = false;
											if (contextRawPointer->m_isSourceReturned.compare_exchange_strong(isBufferedSourceNotReturned, true))
											{
												releasedSources.push_back(contextRawPointer->m_source);
											}
											it = m_playingBufferedSounds.erase(it);
											if (m_playingBufferedSoundsHoldings.find(contextRawPointer) != m_playingBufferedSoundsHoldings.end())
											{
												loopBufferedContextLifeHoldings.push_back(sharedPlayingContext);
												m_playingBufferedSoundsHoldings.erase(contextRawPointer);
											}
										}
										else
										{
											if (contextRawPointer->IsPaused())
											{
												if (m_playingBufferedSoundsHoldings.find(contextRawPointer) != m_playingBufferedSoundsHoldings.end())
												{
													loopBufferedContextLifeHoldings.push_back(sharedPlayingContext);
													m_playingBufferedSoundsHoldings.erase(contextRawPointer);
												}
											}
											else
											{
												if (m_playingBufferedSoundsHoldings.find(contextRawPointer) == m_playingBufferedSoundsHoldings.end())
												{
													m_playingBufferedSoundsHoldings.insert(std::make_pair(contextRawPointer, sharedPlayingContext));
												}
											}
											++it;
										}
									}
									else
									{
										it = m_playingBufferedSounds.erase(it);
									}
								}

								for (std::list<std::shared_ptr<_Internal::OpenALSourceReleasingContext> >::iterator it = m_releasingContexts.begin(); it != m_releasingContexts.end(); )
								{
									(*it)->Update(releasedBuffers);
									if ((*it)->IsReleaseCompleted())
									{
										releasedSources.push_back((*it)->m_source);
										it = m_releasingContexts.erase(it);
									}
									else
									{
										++it;
									}
								}
							}

							if (!releasedBuffers.empty())
							{
								_ReleaseBuffers(&(releasedBuffers[0]), releasedBuffers.size());
								releasedBuffers.clear();
							}
							if (!releasedSources.empty())
							{
								_ReleaseSources(releasedSources);
								releasedSources.clear();
							}

							loopContextLifeHoldings.clear();
							loopBufferedContextLifeHoldings.clear();
						}
					}
					else
					{
						// Don't do anything w/o a sure of non-suspended state
					}

					m_lastSignalTimeInEpochMicroSec.store((long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());

					threadCalculationDuration = std::chrono::steady_clock::now() - beforeThreadCalculation;
					long threadCalculationDurationInNS = (long)std::chrono::duration_cast<std::chrono::nanoseconds>(threadCalculationDuration).count();
					if (threadCalculationDurationInNS < 0)
					{
						// Defence fail-over code for the situation Steady Clock reversed
						threadCalculationDurationInNS = 0;
					}
					long realSleepDuration = loopSleepLimitTimeInNS - threadCalculationDurationInNS;
					if (realSleepDuration < loopSleepMinimumTimeInNS)
					{
						Utility::Threading::PthreadSleep(loopSleepMinimumTimeInNS);
					}
					else
					{
						loopSleepLimitTime.tv_nsec = realSleepDuration;
						currentWaitTimeBuffer = Utility::Threading::TimespecAdd(Utility::Threading::GetCurrentSystemTimespec(), loopSleepLimitTime);

						int loopCondLockResult = pthread_mutex_lock(&m_updateLoopConsumeMutex.GetLock());
						if (loopCondLockResult == 0)
						{
							pthread_mutex_t *lockCopy = &m_updateLoopConsumeMutex.GetLock();
							auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
							FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

							pthread_cond_timedwait(&m_updateLoopConsumeCondition.GetConditionVariable(), &m_updateLoopConsumeMutex.GetLock(), &currentWaitTimeBuffer);
						}
						else
						{
							Utility::Threading::PthreadSleep(loopSleepLimitTimeInNS);
						}
					}
				}

				_CleanUpContexts();
			}

			//------------------------------------------------------------------------------
			// Sound Play Thread Helper Function
			void *OpenALSoundMixer::_UpdatePlayThreadHelper(void *threadThis)
			{
				((OpenALSoundMixer *)threadThis)->_UpdatePlay();
				return nullptr;
			}

			//------------------------------------------------------------------------------
			// Check whether is it too short time interval, and signal update thread
			void OpenALSoundMixer::_CheckAndSignalUpdate()
			{
				constexpr long long minimumUpdatePeriodInMicroSec = 2000; // 2 ms

				long long currentTimeValue = (long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				if (currentTimeValue > minimumUpdatePeriodInMicroSec + m_lastSignalTimeInEpochMicroSec.load())
				{
					m_lastSignalTimeInEpochMicroSec.store(currentTimeValue);
					pthread_cond_signal(&m_updateLoopConsumeCondition.GetConditionVariable());
				}
			}


			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_NotifyPlayingSoundResume(std::shared_ptr<SoundPlayingContext> context)
			{
				if (!context)
				{
					return;
				}

				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *soundListLockCopy = &m_soundListMutex.GetLock();
					auto releaseSoundListLockFunc = [soundListLockCopy]() mutable { pthread_mutex_unlock(soundListLockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_SOUNDLIST_LOCK, releaseSoundListLockFunc);

					SoundPlayingContext *contextRawPointer = context.get();
					if (m_playingSoundsHoldings.find(contextRawPointer) == m_playingSoundsHoldings.end())
					{
						m_playingSoundsHoldings.insert(std::make_pair(contextRawPointer, context));
					}
				}
				else
				{
					// Don't processing without lock
				}
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_NotifyPlayingBufferedSoundResume(std::shared_ptr<BufferedSoundPlayingContext> context)
			{
				if (!context)
				{
					return;
				}

				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *soundListLockCopy = &m_soundListMutex.GetLock();
					auto releaseSoundListLockFunc = [soundListLockCopy]() mutable { pthread_mutex_unlock(soundListLockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_SOUNDLIST_LOCK, releaseSoundListLockFunc);

					BufferedSoundPlayingContext *contextRawPointer = context.get();
					if (m_playingBufferedSoundsHoldings.find(contextRawPointer) == m_playingBufferedSoundsHoldings.end())
					{
						m_playingBufferedSoundsHoldings.insert(std::make_pair(contextRawPointer, context));
					}
				}
				else
				{
					// Don't processing without lock
				}
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_PushSourceToRelease(ALuint source)
			{
				std::shared_ptr<_Internal::OpenALSourceReleasingContext> releasingContext = std::make_shared<_Internal::OpenALSourceReleasingContext>(source);

				int lockResult = pthread_mutex_lock(&m_soundListMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *soundListLockCopy = &m_soundListMutex.GetLock();
					auto releaseSoundListLockFunc = [soundListLockCopy]() mutable { pthread_mutex_unlock(soundListLockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_SOUNDLIST_LOCK, releaseSoundListLockFunc);

					m_releasingContexts.push_back(releasingContext);
				}
				else
				{
					// Don't processing without lock
				}
			}

			//------------------------------------------------------------------------------
			void OpenALSoundMixer::_CleanUpContexts()
			{
				constexpr long _cleanUpLoopWaitingTimeInMS = 20;

				int lockResult = pthread_mutex_lock(&m_cleanUpMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_cleanUpMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					if (m_isCleanUpDone)
					{
						return;
					}
					m_isCleanUpDone = true;
				}
				else
				{
					// Lock Failed
					if (m_isCleanUpDone)
					{
						return;
					}
					m_isCleanUpDone = true;
				}

				for (std::list<std::weak_ptr<BufferedSoundPlayingContext> >::iterator it = m_playingBufferedSounds.begin(); it != m_playingBufferedSounds.end(); ++it)
				{
					std::shared_ptr<BufferedSoundPlayingContext> sharedPlayingContext(it->lock());
					if (!sharedPlayingContext)
					{
						continue;
					}
					sharedPlayingContext->Stop();
				}
				m_playingBufferedSounds.clear();
				m_playingBufferedSoundsHoldings.clear();

				for (std::list<std::weak_ptr<SoundPlayingContext> >::iterator it = m_playingSounds.begin(); it != m_playingSounds.end(); ++it)
				{
					std::shared_ptr<SoundPlayingContext> playingContext(it->lock());
					if (!playingContext)
					{
						continue;
					}
					playingContext->Stop();
				}
				m_playingSounds.clear();
				m_playingSoundsHoldings.clear();

				m_releasingContexts.clear();
			}

			//------------------------------------------------------------------------------
			size_t OpenALSoundMixer::GetDefaultBufferForSoundCount()
			{
				return _Internal::kSoundDefaultBufferCountPerSound;
			}
		}
	}
}
