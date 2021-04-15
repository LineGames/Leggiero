////////////////////////////////////////////////////////////////////////////////
// ConcreteTaskManager.cpp (Leggiero/Modules - Task)
//
// Real Task Manager Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_Internal/_ConcreteTaskManager.h"

// Standard Library
#include <algorithm>

// Leggiero.Utility
#include <Utility/Math/BitMath.h>
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ThreadSleep.h>

// Leggiero.Engine
#include <Engine/Application/GameProcessAnchor.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventObserver.h>
#include <Application/SystemEventDispatcher.h>

// Leggiero.Task
#include "_Internal/_TaskExecutionEntry.h"
#include "TaskSubSystemInterface.h"
#include "Processor/ITaskProcessor.h"
#include "Tasks/ITask.h"


namespace Leggiero
{
	namespace Task
	{
		namespace _Internal
		{
			//////////////////////////////////////////////////////////////////////////////// ConcreteTaskManager

			//------------------------------------------------------------------------------
			ConcreteTaskManager::ConcreteTaskManager()
				: m_isRealtimeScheduling(false), m_conditionWaitingQueueProcessing(false)
				, m_conditionCheckShortTermIndex(-1), m_conditionCheckLongTermIndex(-1)
				, m_generalTaskCapability(TaskCapabilities::kGeneral)
			{
			}

			//------------------------------------------------------------------------------
			ConcreteTaskManager::~ConcreteTaskManager()
			{
				_FinalizeSchedulingQueues();
				_FinalizeProcessors();

				TaskExecutionEntry *deletingExecutionEntry;
				while (m_executionEntryPool.try_dequeue(deletingExecutionEntry))
				{
					delete deletingExecutionEntry;
				}
			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void ConcreteTaskManager::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
				m_isRealtimeScheduling.store(false);
				m_conditionWaitingQueueProcessing.store(false);

				for (std::shared_ptr<ITaskSubSystem> &subSystem : m_subSystems)
				{
					subSystem->InitializeSubSystem(this, m_appComponentCopy, gameAnchor);
				}

				_InitializeProcessors();
				_InitializeSchedulingQueues();

				m_appComponentCopy->SystemEventCenter().RegisterApplicationEventObserver(this);

				m_gameAnchor = gameAnchor;
				gameAnchor->RegisterOnAfterFrame(this);
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void ConcreteTaskManager::ShutdownComponent()
			{
				m_gameAnchor->UnRegisterOnAfterFrame(this);
				m_appComponentCopy->SystemEventCenter().UnRegisterApplicationEventObserver(this);

				_FinalizeSchedulingQueues();
				_FinalizeProcessors();

				for (std::shared_ptr<ITaskSubSystem> &subSystem : m_subSystems)
				{
					subSystem->ShutdownSubSystem();
				}
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent modules needed by this component
			const std::vector<EngineModuleIdType> &ConcreteTaskManager::GetDependentModules() const
			{
				std::set<EngineModuleIdType> dependentModules;

				for (const std::shared_ptr<ITaskSubSystem> &subSystem : m_subSystems)
				{
					subSystem->CollectModuleDependency(dependentModules);
				}

				static std::vector<EngineModuleIdType> collectedIdList;
				collectedIdList.clear();
				collectedIdList.insert(collectedIdList.end(), dependentModules.begin(), dependentModules.end());

				return collectedIdList;
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent components needed by this component
			const std::vector<EngineComponentIdType> &ConcreteTaskManager::GetDependentComponents() const
			{
				static std::vector<EngineComponentIdType> dependentComponentList{
					EngineComponentIdType::kApplication
				};

				std::set<EngineComponentIdType> dependentComponents;
				dependentComponents.insert(dependentComponentList.begin(), dependentComponentList.end());

				for (const std::shared_ptr<ITaskSubSystem> &subSystem : m_subSystems)
				{
					subSystem->CollectComponentDependency(dependentComponents);
				}

				static std::vector<EngineComponentIdType> collectedIdList;
				collectedIdList.clear();
				collectedIdList.insert(collectedIdList.end(), dependentComponents.begin(), dependentComponents.end());

				return collectedIdList;
			}

			//------------------------------------------------------------------------------
			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			void ConcreteTaskManager::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
			{
				switch (componentId)
				{
					case EngineComponentIdType::kApplication:
						{
							m_appComponentCopy = dynamic_cast<Application::ApplicationComponent *>(dependentComponent);
						}
						break;
				}

				for (std::shared_ptr<ITaskSubSystem> &subSystem : m_subSystems)
				{
					subSystem->InjectComponentDependency(componentId, dependentComponent);
				}
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::AttachSubSystem(std::shared_ptr<ITaskSubSystem> subSystem)
			{
				if (!subSystem)
				{
					return;
				}

				m_subSystems.push_back(subSystem);
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::AddGeneralWorkerCapability(TaskCapabilityType capabilities)
			{
				m_generalTaskCapability |= capabilities;
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::RequestExecution(TaskExecutionEntry *execution)
			{
				if (!execution->task->IsTaskReady())
				{
					// Should wait ready
					switch (execution->task->GetTaskPriority())
					{
						case TaskPriorityClass::kBackground:
							m_conditionWaitingQueueLongTerm.enqueue(execution);
							break;

						case TaskPriorityClass::kHighPriority:
							m_conditionWaitingQueueRealtime.enqueue(execution);
							break;

						default:
							m_conditionWaitingQueueShortTerm.enqueue(execution);
							break;
					}
					return;
				}

				_DispatchExecution(execution);
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::RequestDelayedExecution(TaskExecutionEntry *execution, SchedulingClock::duration delay)
			{
				if (delay <= SchedulingClock::duration::zero())
				{
					// No Delay
					RequestExecution(execution);
					return;
				}

				execution->lastStep = execution->lastStep + delay;
				_EnqueueWaitingExecution(execution);
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::ReleaseExecution(TaskExecutionEntry *execution)
			{
				if (execution == nullptr)
				{
					return;
				}
				execution->Clear();

				if (!m_executionEntryPool.enqueue(execution))
				{
					delete execution;
				}
			}

			//------------------------------------------------------------------------------
			bool ConcreteTaskManager::ExecuteTask(std::shared_ptr<ITask> task)
			{
				if (!task)
				{
					return false;
				}

				if (_GetTaskProcessor(task->GetRequiredCapabilties()) == nullptr)
				{
					// Cannot process
					return false;
				}

				TaskExecutionEntry *execution = _RetainExecution();
				if (execution == nullptr)
				{
					return false;
				}
				execution->task = task;

				RequestExecution(execution);
				_HintRealtimeSchedule();

				return true;
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::OnGoToBackground()
			{
				m_schedulerPause.store(true);
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::OnReturnFromBackground()
			{
				m_schedulerPause.store(false);
				pthread_cond_broadcast(&m_queuesPauseCond.GetConditionVariable());
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::GameProcess_OnAfterFrame(GameFrameNumberType frameNumber)
			{
				_HintRealtimeSchedule();
				_ProcessConditionWaitingQueue(m_conditionWaitingQueueShortTerm);

				if (frameNumber % 8 == 0)
				{
					// Long-term condition check every 8 frames
					_ProcessConditionWaitingQueue(m_conditionWaitingQueueLongTerm);
				}
			}

			//------------------------------------------------------------------------------
			TaskExecutionEntry *ConcreteTaskManager::_RetainExecution()
			{
				TaskExecutionEntry *pooledExecutionEntry;
				if (m_executionEntryPool.try_dequeue(pooledExecutionEntry))
				{
					return pooledExecutionEntry;
				}

				return new TaskExecutionEntry();
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_DispatchExecution(TaskExecutionEntry *execution)
			{
				ITaskProcessor *taskProcessor = _GetTaskProcessor(execution->task->GetRequiredCapabilties());
				if (taskProcessor == nullptr)
				{
					// ???
					execution->task->SetErrorFlag();
					execution->task->State().store(TaskState::kError);
					ReleaseExecution(execution);
					return;
				}

				taskProcessor->GiveJob(execution);
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_InitializeProcessors()
			{
				_RegisterProcessor(_CreateGeneralProcessor());

				for (std::shared_ptr<ITaskSubSystem> &subSystem : m_subSystems)
				{
					subSystem->CreateProcessors([this](std::shared_ptr<ITaskProcessor> processor) { _RegisterProcessor(processor); });
				}
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_FinalizeProcessors()
			{
				m_capabilityProcessorMap.clear();
				m_capabilityOnlyList.clear();

				for (std::shared_ptr<ITaskProcessor> &currentProcessor : m_taskProcessorHolder)
				{
					currentProcessor->PrepareProcessorShutdown();
				}

				// Just delete processor by clear shared_ptr holder
				m_taskProcessorHolder.clear();
			}

			//------------------------------------------------------------------------------
			// Should not called in race-condition with _GetTaskProcessor (only for _InitializeProcessors step)
			void ConcreteTaskManager::_RegisterProcessor(std::shared_ptr<ITaskProcessor> processor)
			{
				if (!processor)
				{
					return;
				}

				m_taskProcessorHolder.push_back(processor);

				TaskCapabilityType processorCapability = processor->GetProcessorTaskCapability();
				m_capabilityOnlyList.push_back(processorCapability);

				std::unordered_map<TaskCapabilityType, ITaskProcessor *>::iterator findIt = m_capabilityProcessorMap.find(processorCapability);
				if (findIt == m_capabilityProcessorMap.end())
				{
					m_capabilityProcessorMap.insert(std::make_pair(processorCapability, processor.get()));
				}
			}

			//------------------------------------------------------------------------------
			ITaskProcessor *ConcreteTaskManager::_GetTaskProcessor(TaskCapabilityType capability)
			{
				// Calculate every time to avoid locking

				std::unordered_map<TaskCapabilityType, ITaskProcessor *>::iterator findIt = m_capabilityProcessorMap.find(capability);
				if (findIt != m_capabilityProcessorMap.end())
				{
					return findIt->second;
				}

				const int processorCount = static_cast<int>(m_capabilityOnlyList.size());

				int matchIndex = -1;
				int matchMinDiffBits = 1000;	// may not run in 1024-bit word machine? lol
				for (int i = 0; i < processorCount; ++i)
				{
					// Check Requirements
					bool isMatch = (capability & (~(m_capabilityOnlyList[i] | m_generalTaskCapability))) == 0;
					if (!isMatch)
					{
						continue;
					}

					// Check different bits
					TaskCapabilityType diffMap = (capability ^ m_capabilityOnlyList[i]);
					int diffBits = static_cast<int>(Utility::Math::PopCount(diffMap));
					if (diffBits < matchMinDiffBits)
					{
						matchIndex = i;
						matchMinDiffBits = diffBits;
						if (matchMinDiffBits <= 1)
						{
							break;
						}
					}
				}

				if (matchIndex >= 0)
				{
					return m_taskProcessorHolder[matchIndex].get();
				}

				return nullptr;
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_HintRealtimeSchedule()
			{
				if (m_isRealtimeScheduling.load())
				{
					return;
				}

				bool notScheduling = false;
				if (m_isRealtimeScheduling.compare_exchange_strong(notScheduling, true))
				{
					_RealtimeSchedule();
					_ProcessConditionWaitingQueue(m_conditionWaitingQueueRealtime);
					m_isRealtimeScheduling.store(false);
				}
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_RealtimeSchedule()
			{
				TaskExecutionEntry *dequeuedExecution;
				size_t executionBatchCount = 0;
				while (m_realtimeWaitingQueue.try_dequeue(dequeuedExecution))
				{
					m_realtimeSchedulingBuffer[executionBatchCount] = dequeuedExecution;
					++executionBatchCount;
					if (executionBatchCount >= kRealtimeScheduleMaxBatchCount)
					{
						break;
					}
				}

				for (size_t i = 0; i < executionBatchCount; ++i)
				{
					TaskExecutionEntry *currentExecution = m_realtimeSchedulingBuffer[i];
					if (currentExecution->lastStep <= SchedulingClock::now())
					{
						RequestExecution(currentExecution);
						m_realtimeSchedulingBuffer[i] = nullptr;
					}
				}

				for (size_t i = 0; i < executionBatchCount; ++i)
				{
					if (m_realtimeSchedulingBuffer[i] != nullptr)
					{
						m_realtimeWaitingQueue.enqueue(m_realtimeSchedulingBuffer[i]);
					}
				}
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_ProcessConditionWaitingQueue(moodycamel::ConcurrentQueue<TaskExecutionEntry *> &queue)
			{
				if (m_conditionWaitingQueueProcessing.load())
				{
					return;
				}

				bool notProcessing = false;
				if (m_conditionWaitingQueueProcessing.compare_exchange_strong(notProcessing, true))
				{
					std::atomic_bool *lockCopy = &m_conditionWaitingQueueProcessing;
					auto releaseLockFunc = [lockCopy]() mutable { lockCopy->store(false); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					m_conditionWaitingProcessBuffer.reserve(queue.size_approx() + 4);

					TaskExecutionEntry *dequeuedExecution;
					while (queue.try_dequeue(dequeuedExecution))
					{
						m_conditionWaitingProcessBuffer.push_back(dequeuedExecution);
					}

					for (TaskExecutionEntry *currentExecution : m_conditionWaitingProcessBuffer)
					{
						if (currentExecution->task->IsTaskReady())
						{
							_DispatchExecution(currentExecution);
						}
						else
						{
							queue.enqueue(currentExecution);
						}
					}
					m_conditionWaitingProcessBuffer.clear();
				}
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_InitializeSchedulingQueues()
			{
				// Queue Definitions
				static std::vector<SchedulingClock::duration> queueWaitingTimes{
					std::chrono::milliseconds(2), 
					std::chrono::milliseconds(8),
					std::chrono::milliseconds(32),
					std::chrono::milliseconds(128),
					std::chrono::milliseconds(1024),
				};
				std::sort(queueWaitingTimes.begin(), queueWaitingTimes.end());

				m_schedulerRunning.store(true);
				m_schedulerPause.store(false);

				m_schedulingQueues.reserve(queueWaitingTimes.size());
				for (int i = 0; i < queueWaitingTimes.size(); ++i)
				{
					SchedulingQueueEntry *currentQueueEntry = new SchedulingQueueEntry();
					currentQueueEntry->minimumWaiting = queueWaitingTimes[i];
					currentQueueEntry->owner = this;
					currentQueueEntry->queueIndex = i;
					m_schedulingQueues.push_back(currentQueueEntry);
				}

				for (SchedulingQueueEntry *currentQueueEntry : m_schedulingQueues)
				{
					if (pthread_create(&currentQueueEntry->schedulingThread, NULL, ConcreteTaskManager::_SchedulingQueueThreadStartHelper, (void *)currentQueueEntry) != 0)
					{
						// Thread Creation Failed
					}
				}

				m_schedulingQueuesCount = static_cast<int>(m_schedulingQueues.size());

				m_conditionCheckShortTermIndex = 1;
				m_conditionCheckLongTermIndex = 3;
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_FinalizeSchedulingQueues()
			{
				m_schedulerPause.store(false);
				m_schedulerRunning.store(false);

				pthread_cond_broadcast(&m_queuesPauseCond.GetConditionVariable());

				if (m_schedulingQueues.empty())
				{
					return;
				}

				for (SchedulingQueueEntry *currentQueueEntry : m_schedulingQueues)
				{
					pthread_join(currentQueueEntry->schedulingThread, NULL);
					delete currentQueueEntry;
				}
				m_schedulingQueues.clear();
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_SchedulingQueueThreadFunction(ConcreteTaskManager::SchedulingQueueEntry &queueEntry)
			{
				std::chrono::duration<int, std::micro> microsecMinimumWaiting(std::chrono::duration_cast<std::chrono::duration<int, std::micro> >(queueEntry.minimumWaiting));
				int waitingMicroseconds = microsecMinimumWaiting.count() / 2;
				if (waitingMicroseconds <= 0)
				{
					waitingMicroseconds = 1;
				}
				
				std::chrono::nanoseconds startNs;
				std::chrono::nanoseconds finishNs;
				while (m_schedulerRunning.load())
				{
					startNs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());

					// Process Pause
					if (m_schedulerPause.load())
					{
						int pauseCondLockResult = pthread_mutex_lock(&(m_queuesPauseMutex.GetLock()));
						if (pauseCondLockResult == 0)
						{
							pthread_mutex_t *lockCopy = &(m_queuesPauseMutex.GetLock());
							auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
							FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

							if (m_schedulerPause.load())
							{
								pthread_cond_wait(&(m_queuesPauseCond.GetConditionVariable()), &(m_queuesPauseMutex.GetLock()));
							}
						}

						// Yield and re-check
						sched_yield();
						continue;
					}

					_DoWaitingQueueScheduling(queueEntry.queueIndex, queueEntry);

					if (queueEntry.queueIndex == m_conditionCheckShortTermIndex)
					{
						_ProcessConditionWaitingQueue(m_conditionWaitingQueueShortTerm);
					}
					else if (queueEntry.queueIndex == m_conditionCheckLongTermIndex)
					{
						_ProcessConditionWaitingQueue(m_conditionWaitingQueueLongTerm);
					}

					_HintRealtimeSchedule();

					finishNs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
					
					auto elapsedNs = (finishNs - startNs).count();
					if (elapsedNs < 0)
					{
						elapsedNs = 0;
					}
					int shouldWaiting = waitingMicroseconds - static_cast<int>(elapsedNs / 1000);

					if (shouldWaiting > 0)
					{
						Utility::Threading::PthreadSleepuS(shouldWaiting);
					}
					else
					{
						sched_yield();
					}
				}
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_DoWaitingQueueScheduling(int queueIndex, ConcreteTaskManager::SchedulingQueueEntry &queueEntry)
			{
				TaskExecutionEntry *dequeuedExecution;
				while (queueEntry.queue.try_dequeue(dequeuedExecution))
				{
					queueEntry.queueProcessingBuffer.push_back(dequeuedExecution);
				}

				for (TaskExecutionEntry *currentExecution : queueEntry.queueProcessingBuffer)
				{
					if (currentExecution->lastStep <= SchedulingClock::now())
					{
						queueEntry.queueProcessingNowBuffer.push_back(currentExecution);
					}
					else
					{
						// Re-dispatch to waiting queue
						_EnqueueWaitingExecution(currentExecution);
					}
				}
				queueEntry.queueProcessingBuffer.clear();

				for (TaskExecutionEntry *currentExecution : queueEntry.queueProcessingNowBuffer)
				{
					RequestExecution(currentExecution);
				}
				queueEntry.queueProcessingNowBuffer.clear();
			}

			//------------------------------------------------------------------------------
			void *ConcreteTaskManager::_SchedulingQueueThreadStartHelper(void *threadEntry)
			{
				SchedulingQueueEntry *queueEntry = (SchedulingQueueEntry *)threadEntry;
				queueEntry->owner->_SchedulingQueueThreadFunction(*queueEntry);
				return nullptr;
			}

			//------------------------------------------------------------------------------
			void ConcreteTaskManager::_EnqueueWaitingExecution(TaskExecutionEntry *execution)
			{
				SchedulingClock::duration waitingTime = execution->lastStep - SchedulingClock::now();
				
				if (m_schedulingQueues.empty() || (waitingTime <= m_schedulingQueues.front()->minimumWaiting))
				{
					// Realtime scheduling
					m_realtimeWaitingQueue.enqueue(execution);
					return;
				}

				// Linear search because queues are not too many
				int targetQueueIndex = 0;
				for (int i = 1; i < m_schedulingQueuesCount; ++i)
				{
					if (waitingTime > m_schedulingQueues[i]->minimumWaiting)
					{
						targetQueueIndex = i;
					}
					else
					{
						break;
					}
				}

				m_schedulingQueues[targetQueueIndex]->queue.enqueue(execution);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// TaskManagerComponent - Factory

		//------------------------------------------------------------------------------
		// Create function to create concrete object
		TaskManagerComponent *TaskManagerComponent::CreateComponentObject()
		{
			return new _Internal::ConcreteTaskManager();
		}
	}
}
