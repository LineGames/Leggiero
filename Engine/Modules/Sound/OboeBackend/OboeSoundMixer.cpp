////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeSoundMixer.cpp (Leggiero/Modules - Sound)
//
// Sound Mixer Implementation in Oboe Backend
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeSoundMixer.h"

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
#include "OboeStream.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			//////////////////////////////////////////////////////////////////////////////// OboeSoundMixer

			//------------------------------------------------------------------------------
			OboeSoundMixer::OboeSoundMixer()
				: m_isOnceInitialized(false), m_appComponentCopy(nullptr)
				, m_isSuspended(true), m_isShutdownRequested(false), m_isUpdateThreadExists(false), m_isCleanUpDone(false)
				, m_lastSignalTimeInEpochMicroSec((long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
				, m_myPointerHolder(std::make_shared<Utility::Object::PointerHolder>(this))
			{
				
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::_StartSoundUpdateThread()
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
						// Update Thread Creation Failed
						m_isUpdateThreadExists = false;

						// Can be failed by permission
						if (pthread_create(&m_soundUpdateThread, NULL, _UpdatePlayThreadHelper, this) != 0)
						{
							// Update Thread Creation Fallback Failed
							m_isUpdateThreadExists = false;
							
							// FAILOVER PROCESS IMPLEMENTATION NEEDED
						}
						else
						{
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
			OboeSoundMixer::~OboeSoundMixer()
			{
				m_myPointerHolder->NotifyTargetDeleted();
				ShutdownComponent();
			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void OboeSoundMixer::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
				const std::vector<SamplingFrequencyType> &preparingFrequencies = GetPreparingFrequencies();
				for (SamplingFrequencyType currentFrequency : preparingFrequencies)
				{
					_InitializeStream(currentFrequency, false);
				}

				m_isSuspended = false;

				_StartSoundUpdateThread();

				if (m_appComponentCopy != nullptr)
				{
					m_appComponentCopy->SystemEventCenter().RegisterSystemEventObserver(this);
				}

				m_isOnceInitialized.store(true);
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void OboeSoundMixer::ShutdownComponent()
			{
				bool isInitialized = true;
				if (!m_isOnceInitialized.compare_exchange_strong(isInitialized, false))
				{
					// Only one shutdown for a starting
					return;
				}

				if (m_appComponentCopy != nullptr)
				{
					m_appComponentCopy->SystemEventCenter().UnRegisterSystemEventObserver(this);
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

				// Clear Underlying Streams
				for (std::unordered_map<int32_t, OboeStream *>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
				{
					delete it->second;
				}
				m_streams.clear();
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent components needed by this component
			const std::vector<EngineComponentIdType> &OboeSoundMixer::GetDependentComponents() const
			{
				static std::vector<EngineComponentIdType> dependentComponentList{
					EngineComponentIdType::kApplication
				};
				return dependentComponentList;
			}

			//------------------------------------------------------------------------------
			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			void OboeSoundMixer::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
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
			void OboeSoundMixer::_SuspendMixer()
			{
				int lockResult = pthread_mutex_lock(&m_suspendMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_suspendMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_isSuspended = true;
				}
				else
				{
					// Anyway, Go
					m_isSuspended = true;
				}

				pthread_cond_signal(&m_updateLoopConsumeCondition.GetConditionVariable());
				pthread_join(m_soundUpdateThread, nullptr);
				m_isUpdateThreadExists = false;

				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_streamsLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *listLock = &m_streamsLock.GetLock();
					auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					for (std::unordered_map<int32_t, OboeStream *>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
					{
						it->second->StopStream();
					}
				}
				else
				{
					// Ignore when lock failed
				}
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::_RestoreMixer()
			{
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

				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_streamsLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *listLock = &m_streamsLock.GetLock();
					auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					for (std::unordered_map<int32_t, OboeStream *>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
					{
						it->second->StartStream();
					}
				}
				else
				{
					// Ignore when lock failed
				}

				_StartSoundUpdateThread();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<SoundPlayingContext> OboeSoundMixer::Play(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately, float volume, bool isPauseAfterFinish)
			{
				if (sound == nullptr)
				{
					return nullptr;
				}
				std::shared_ptr<SoundPlayingContext> playingContext = std::make_shared<SoundPlayingContext>(sound, m_myPointerHolder, isStartImmediately, volume, isPauseAfterFinish);
				if (!playingContext)
				{
					return nullptr;
				}

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
					return nullptr;
				}

				_RegisterStreamSound(playingContext, playingContext->m_soundSampleFrequency);

				_CheckAndSignalUpdate();

				return playingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<LoopingSoundPlayingContext> OboeSoundMixer::PlayLoopingSound(std::shared_ptr<ISoundProvider> sound, SampleNumberType loopStartSamplePosition, SampleNumberType loopFinishSamplePosition, bool isStartImmediately, float volume)
			{
				if (sound == nullptr)
				{
					return nullptr;
				}

				std::shared_ptr<LoopingSoundPlayingContext> playingContext = std::make_shared<LoopingSoundPlayingContext>(sound, m_myPointerHolder, loopStartSamplePosition, loopFinishSamplePosition, isStartImmediately, volume);
				if (!playingContext)
				{
					return nullptr;
				}

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
					return nullptr;
				}

				_RegisterStreamSound(playingContext, playingContext->m_soundSampleFrequency);

				_CheckAndSignalUpdate();

				return playingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<BufferedSoundPlayingContext> OboeSoundMixer::PlayBufferedSound(std::shared_ptr<ISoundProvider> sound, bool isStartImmediately, float volume, bool isLooping, bool isPauseAfterFinish)
			{
				if (sound == nullptr)
				{
					return nullptr;
				}

				std::shared_ptr<BufferedSoundPlayingContext> playingContext = std::make_shared<BufferedSoundPlayingContext>(sound, m_myPointerHolder, isStartImmediately, volume, isLooping, isPauseAfterFinish);
				if (!playingContext)
				{
					return nullptr;
				}

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

				_RegisterStreamSound(playingContext, playingContext->m_soundSampleFrequency);

				_CheckAndSignalUpdate();

				return playingContext;
			}

			//------------------------------------------------------------------------------
			// Clear all non-managed(user having handle) sounds
			void OboeSoundMixer::ClearNonManagedSounds()
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
			void OboeSoundMixer::HintUpdate()
			{
				m_lastSignalTimeInEpochMicroSec.store((long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
				pthread_cond_signal(&m_updateLoopConsumeCondition.GetConditionVariable());
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::OnGoToBackground()
			{
				_SuspendMixer();
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::OnReturnFromBackground()
			{
				_RestoreMixer();
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::OnMemoryWarning()
			{
				// Compact Buffers
				m_segmentedBufferPool.Clear();
				m_fullBufferPool.Clear();
			}

			//------------------------------------------------------------------------------
			// (Sound Play Thread) Update currently playing sounds
			void OboeSoundMixer::_UpdatePlay()
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
										contextRawPointer->_Update();
										if (contextRawPointer->IsFinished())
										{
											it = m_playingSounds.erase(it);
											if (m_playingSoundsHoldings.find(contextRawPointer) != m_playingSoundsHoldings.end())
											{
												m_playingSoundsHoldings.erase(contextRawPointer);
											}
										}
										else
										{
											if (contextRawPointer->IsPaused())
											{
												if (m_playingSoundsHoldings.find(contextRawPointer) != m_playingSoundsHoldings.end())
												{
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
											it = m_playingBufferedSounds.erase(it);
											if (m_playingBufferedSoundsHoldings.find(contextRawPointer) != m_playingBufferedSoundsHoldings.end())
											{
												m_playingBufferedSoundsHoldings.erase(contextRawPointer);
											}
										}
										else
										{
											if (contextRawPointer->IsPaused())
											{
												if (m_playingBufferedSoundsHoldings.find(contextRawPointer) != m_playingBufferedSoundsHoldings.end())
												{
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
							}
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
					
					// Update Streams
					int readLockResult;
					while ((readLockResult = pthread_rwlock_rdlock(&m_streamsLock.GetLock())) == EAGAIN)
					{
						sched_yield();
					}
					if (readLockResult == 0)
					{
						pthread_rwlock_t *listLock = &m_streamsLock.GetLock();
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

						for (std::unordered_map<int32_t, OboeStream *>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
						{
							it->second->_MakeRenderQueue();
						}
					}
					else
					{
						// Ignore when lock failed
					}
				}

				_CleanUpContexts();
			}

			//------------------------------------------------------------------------------
			// Sound Play Thread Helper Function
			void *OboeSoundMixer::_UpdatePlayThreadHelper(void *threadThis)
			{
				((OboeSoundMixer *)threadThis)->_UpdatePlay();
				return nullptr;
			}

			//------------------------------------------------------------------------------
			// Check whether is it too short time interval, and signal update thread
			void OboeSoundMixer::_CheckAndSignalUpdate()
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
			void OboeSoundMixer::_RegisterStreamSound(std::shared_ptr<IOboeStreamRendering> sound, int32_t samplingFrequency)
			{
				OboeStream *pTargetStream = nullptr;

				while (pTargetStream == nullptr)
				{
					int readLockResult;
					while ((readLockResult = pthread_rwlock_rdlock(&m_streamsLock.GetLock())) == EAGAIN)
					{
						sched_yield();
					}
					if (readLockResult == 0)
					{
						pthread_rwlock_t *listLock = &m_streamsLock.GetLock();
						auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
						FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

						std::unordered_map<int32_t, OboeStream *>::iterator streamFindIt = m_streams.find(samplingFrequency);
						if (streamFindIt != m_streams.end())
						{
							pTargetStream = streamFindIt->second;
						}
					}
					else
					{
						// Can be missed when lock failed...
						return;
					}
					if (pTargetStream == nullptr)
					{
						_InitializeStream(samplingFrequency, true);
					}
				}

				pTargetStream->_RegisterRendering(sound);
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::_InitializeStream(int32_t samplingFrequency, bool isInLock)
			{
				if (!isInLock)
				{
					std::unordered_map<int32_t, OboeStream *>::iterator streamFindIt = m_streams.find(samplingFrequency);
					if (streamFindIt != m_streams.end())
					{
						return;
					}

					OboeStream *creatingStream = new OboeStream(samplingFrequency);
					creatingStream->StartStream();
					m_streams.insert(std::make_pair(samplingFrequency, creatingStream));

					return;
				}

				int readLockResult;
				while ((readLockResult = pthread_rwlock_rdlock(&m_streamsLock.GetLock())) == EAGAIN)
				{
					sched_yield();
				}
				if (readLockResult == 0)
				{
					pthread_rwlock_t *listLock = &m_streamsLock.GetLock();
					auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
					FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

					std::unordered_map<int32_t, OboeStream *>::iterator streamFindIt = m_streams.find(samplingFrequency);
					if (streamFindIt != m_streams.end())
					{
						return;
					}
				}
				else
				{
					// Can be missed when lock failed...
					return;
				}

				OboeStream *pTargetStream = new OboeStream(samplingFrequency);

				bool isStreamRegistered = false;
				int lockResult = pthread_rwlock_wrlock(&m_streamsLock.GetLock());
				if (lockResult == 0)
				{
					pthread_rwlock_t *lockCopy = &m_streamsLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_rwlock_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					std::unordered_map<int32_t, OboeStream *>::iterator streamFindIt = m_streams.find(samplingFrequency);
					if (streamFindIt != m_streams.end())
					{
						isStreamRegistered = false;
					}
					else
					{
						m_streams.insert(std::make_pair(samplingFrequency, pTargetStream));
						isStreamRegistered = true;
					}
				}
				else
				{
					// Do not register without lock
				}

				if (isStreamRegistered)
				{
					pTargetStream->StartStream();
				}
				else
				{
					delete pTargetStream;
				}
			}

			//------------------------------------------------------------------------------
			void OboeSoundMixer::_NotifyPlayingSoundResume(std::shared_ptr<SoundPlayingContext> context)
			{
				if (!context)
				{
					return;
				}

				if (!context->IsHighPriorityObject())
				{
					_RegisterStreamSound(context, context->m_soundSampleFrequency);
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
			void OboeSoundMixer::_NotifyPlayingBufferedSoundResume(std::shared_ptr<BufferedSoundPlayingContext> context)
			{
				if (!context)
				{
					return;
				}

				if (!context->IsHighPriorityObject())
				{
					_RegisterStreamSound(context, context->m_soundSampleFrequency);
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
			void OboeSoundMixer::_CleanUpContexts()
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
			}
		}
	}
}
