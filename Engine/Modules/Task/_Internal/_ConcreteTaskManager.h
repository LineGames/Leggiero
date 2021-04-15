////////////////////////////////////////////////////////////////////////////////
// _Internal/_ConcreteTaskManager.h (Leggiero/Modules - Task)
//
// Concrete Task Manager Component Class Definition
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK___INTERNAL___CONCRETE_TASK_MANAGER_H
#define __LM_TASK___INTERNAL___CONCRETE_TASK_MANAGER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>

// External Library
#include <concurrentqueue/concurrentqueue.h>
#include <pthread.h>

// Leggiero.Engine
#include <Engine/Application/GameProcessAnchorObserver.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Task
#include "../TaskManagerComponent.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Application
	{
		class ApplicationComponent;
	}


	namespace Task
	{
		namespace _Internal
		{
			// Concrete Task Manager Class to Implement Separated Interface
			class ConcreteTaskManager
				: public TaskManagerComponent
				, public Application::IApplicationEventObserver
				, public Engine::GameProcessAnchorObserver::IAfterFrameHandler
			{
			public:
				ConcreteTaskManager();
				virtual ~ConcreteTaskManager();

			public:	// EngineComponent
				// Initialize the Component
				virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

				// Safely Shutdown Component
				virtual void ShutdownComponent() override;

				// Get type Id list of dependent modules needed by this component
				virtual const std::vector<EngineModuleIdType> &GetDependentModules() const override;

				// Get type Id list of dependent components needed by this component
				virtual const std::vector<EngineComponentIdType> &GetDependentComponents() const override;

				// Inject Dependency to the Component.
				// All dependency injections will be done before the initialization.
				virtual void InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent) override;

			public:	// ITaskManagerSystemFunctions
				virtual void AttachSubSystem(std::shared_ptr<ITaskSubSystem> subSystem) override;
				virtual void AddGeneralWorkerCapability(TaskCapabilityType capabilities) override;

				virtual void RequestExecution(TaskExecutionEntry *execution) override;
				virtual void RequestDelayedExecution(TaskExecutionEntry *execution, SchedulingClock::duration delay) override;

				virtual void ReleaseExecution(TaskExecutionEntry *execution) override;

			public:	// TaskManagerComponent
				virtual bool ExecuteTask(std::shared_ptr<ITask> task) override;

			public:	// Observers
				virtual void OnGoToBackground() override;
				virtual void OnReturnFromBackground() override;
				virtual void GameProcess_OnAfterFrame(GameFrameNumberType frameNumber) override;

			protected:	// Platform
				std::shared_ptr<ITaskProcessor> _CreateGeneralProcessor();

			protected:
				TaskExecutionEntry *_RetainExecution();

				void _DispatchExecution(TaskExecutionEntry *execution);

			protected:
				Application::ApplicationComponent *m_appComponentCopy;
				Engine::GameProcessAnchor *m_gameAnchor;

				moodycamel::ConcurrentQueue<TaskExecutionEntry *> m_executionEntryPool;

				std::vector<std::shared_ptr<ITaskSubSystem> > m_subSystems;

			protected:
				TaskCapabilityType m_generalTaskCapability;

				std::vector<std::shared_ptr<ITaskProcessor> >				m_taskProcessorHolder;
				std::vector<TaskCapabilityType>								m_capabilityOnlyList;
				std::unordered_map<TaskCapabilityType, ITaskProcessor *>	m_capabilityProcessorMap;

				void _InitializeProcessors();
				void _FinalizeProcessors();

				void _RegisterProcessor(std::shared_ptr<ITaskProcessor> processor);

				ITaskProcessor *_GetTaskProcessor(TaskCapabilityType capability);

			protected:	// Realtime Scheduler
				static constexpr size_t kRealtimeScheduleMaxBatchCount = 1024;

				moodycamel::ConcurrentQueue<TaskExecutionEntry *>	m_realtimeWaitingQueue;

				std::atomic_bool m_isRealtimeScheduling;
				TaskExecutionEntry *m_realtimeSchedulingBuffer[kRealtimeScheduleMaxBatchCount];

				void _HintRealtimeSchedule();
				void _RealtimeSchedule();

			protected:
				moodycamel::ConcurrentQueue<TaskExecutionEntry *>	m_conditionWaitingQueueRealtime;
				moodycamel::ConcurrentQueue<TaskExecutionEntry *>	m_conditionWaitingQueueShortTerm;
				moodycamel::ConcurrentQueue<TaskExecutionEntry *>	m_conditionWaitingQueueLongTerm;

				void _ProcessConditionWaitingQueue(moodycamel::ConcurrentQueue<TaskExecutionEntry *> &queue);
				std::atomic_bool m_conditionWaitingQueueProcessing;
				std::vector<TaskExecutionEntry *> m_conditionWaitingProcessBuffer;

			protected:	// Waiting Queues
				struct SchedulingQueueEntry
				{
				public:
					SchedulingClock::duration							minimumWaiting;
					moodycamel::ConcurrentQueue<TaskExecutionEntry *>	queue;

					pthread_t				schedulingThread;
					ConcreteTaskManager *owner;
					int						queueIndex;

					std::vector<TaskExecutionEntry *> queueProcessingBuffer;
					std::vector<TaskExecutionEntry *> queueProcessingNowBuffer;
				};

				std::vector<SchedulingQueueEntry *> m_schedulingQueues;
				int									m_schedulingQueuesCount;
				std::atomic_bool					m_schedulerRunning;
				std::atomic_bool					m_schedulerPause;

				int m_conditionCheckShortTermIndex;
				int m_conditionCheckLongTermIndex;

				Utility::Threading::SafePthreadLock					m_queuesPauseMutex;
				Utility::Threading::SafePthreadConditionVariable	m_queuesPauseCond;

				void _InitializeSchedulingQueues();
				void _FinalizeSchedulingQueues();

				void _SchedulingQueueThreadFunction(SchedulingQueueEntry &queueEntry);
				void _DoWaitingQueueScheduling(int queueIndex, SchedulingQueueEntry &queueEntry);
				static void *_SchedulingQueueThreadStartHelper(void *threadEntry);

				void _EnqueueWaitingExecution(TaskExecutionEntry *execution);
			};
		}
	}
}

#endif
