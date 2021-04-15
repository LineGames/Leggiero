////////////////////////////////////////////////////////////////////////////////
// Processor/ThreadWorker.cpp (Leggiero/Modules - Task)
//
// Thread Worker Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ThreadWorker.h"

// Leggiero.Utility
#include <Utility/Threading/ThreadSleep.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Task
#include "IThreadWorkerContext.h"
#include "../Tasks/ITask.h"
#include "../_Internal/_TaskExecutionEntry.h"


namespace Leggiero
{
	namespace Task
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Utility

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			struct LoopProcessContext
			{
			public:
				bool	isWorkProcessed;
				int		noWorkCount;

			public:
				LoopProcessContext()
					: isWorkProcessed(false), noWorkCount(0)
				{ }
			};
		}


		//////////////////////////////////////////////////////////////////////////////// ThreadWorker

		//------------------------------------------------------------------------------
		ThreadWorker::ThreadWorker(IThreadWorkerContext *pOwnerContext)
			: m_pOwnerContext(pOwnerContext)
			, m_isShutdownRequested(false), m_isThreadFinished(true)
			, m_isPauseRequested(false), m_isThreadPaused(false)
		{
			if (pthread_create(&m_workerThread, NULL, ThreadWorker::_ThreadStartHelper, (void *)this) != 0)
			{
				// Thread Creation Failed
				m_isThreadFinished.store(true);
				return;
			}

			m_isThreadFinished.store(false);
		}

		//------------------------------------------------------------------------------
		ThreadWorker::~ThreadWorker()
		{
			// Join to Finish
			if (IsRunning())
			{
				RequestStop();
				pthread_join(m_workerThread, NULL);
			}
		}

		//------------------------------------------------------------------------------
		bool ThreadWorker::IsRunning()
		{
			return !m_isThreadFinished.load();
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::RequestStop()
		{
			m_isShutdownRequested.store(true);

			if (IsPaused())
			{
				RequestResume();
				pthread_cond_broadcast(&(m_pOwnerContext->PauseCondition()));
			}
		}

		//------------------------------------------------------------------------------
		bool ThreadWorker::IsPaused()
		{
			if (!m_isThreadFinished.load())
			{
				return true;
			}

			return m_isThreadPaused.load();
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::RequestPause()
		{
			m_isPauseRequested.store(true);
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::RequestResume()
		{
			m_isPauseRequested.store(false);
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::_ThreadFunction()
		{
			bool initializationResult = _InitializeBeforeWork();
			if (!initializationResult)
			{
				// Worker Initialization Failed
				m_isThreadFinished.store(true);
				return;
			}

			_Internal::LoopProcessContext loopContext;
			while (!m_isShutdownRequested.load())
			{
				if (m_isPauseRequested.load())
				{
					_WaitPause();
				}
				else
				{
					TaskExecutionEntry *dequeuedItem = m_pOwnerContext->DequeueJob();
					if (dequeuedItem != nullptr)
					{
						loopContext.isWorkProcessed = true;
						loopContext.noWorkCount = 0;
						_ProcessTask(dequeuedItem, loopContext);
					}
					else
					{
						loopContext.isWorkProcessed = false;
						++loopContext.noWorkCount;
					}

					_WaitLoop(loopContext);
				}

				bool loopProcessResult = _LoopProcess();
				if (!loopProcessResult)
				{
					// Worker Loop Process Error
					break;
				}
			}
			_FinalizeAfterWork();

			m_isThreadFinished.store(true);
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::_ProcessTask(TaskExecutionEntry *execution, _Internal::LoopProcessContext &loopContext)
		{
			ITask *pTask = execution->task.get();
			if (pTask == nullptr)
			{
				m_pOwnerContext->ReleaseNotExecution(execution);
				return;
			}
			if (pTask->IsFinished())
			{
				m_pOwnerContext->ReleaseNotExecution(execution);
				return;
			}
			if (pTask->HasError())
			{
				pTask->State().store(TaskState::kError);
				m_pOwnerContext->HandleError(execution);
				return;
			}

			// Before Start Process
			if (!Utility::SyntacticSugar::HasFlag(pTask->GetTaskState(), TaskState::kJobStarted))
			{
				pTask->OnBeforeProcess();
				pTask->State().store(pTask->GetTaskState() | TaskState::kJobStarted);
			}
			if (pTask->HasError())
			{
				pTask->State().store(TaskState::kError);
				m_pOwnerContext->HandleError(execution);
				return;
			}

			// Do Main Task
			pTask->State().store(pTask->GetTaskState() | TaskState::kJobProcessing);

			_PreProcessBeforeTask();
			pTask->OnBeforeStepProcess();

			TaskDoneResult taskResult = pTask->Do();
			execution->lastStep = SchedulingClock::now();

			pTask->OnAfterStepProcess();
			_PostProcessAfterTask();

			pTask->State().store(pTask->GetTaskState() & (~TaskState::kJobProcessing));
			if (pTask->HasError())
			{
				pTask->State().store(TaskState::kError);
				m_pOwnerContext->HandleError(execution);
				return;
			}

			// Process Scheduling
			switch (taskResult.result)
			{
				case TaskDoneResult::ResultType::kFinished:
					{
						// Done
						pTask->OnAfterProcess();

						if (pTask->HasError())
						{
							pTask->State().store(TaskState::kError);
							m_pOwnerContext->HandleError(execution);
							return;
						}
						else
						{
							// Finished
							pTask->State().store(TaskState::kDone);
							m_pOwnerContext->HandleDone(execution);
						}
					}
					break;

				case TaskDoneResult::ResultType::kYield:
					{
						m_pOwnerContext->HandleYield(execution);
					}
					break;

				case TaskDoneResult::ResultType::kSleep:
				case TaskDoneResult::ResultType::kWaitCondition:
					{
						m_pOwnerContext->HandleWait(execution, taskResult);
					}
					break;
			}
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::_WaitLoop(_Internal::LoopProcessContext &loopContext)
		{
			// Fast loop
			if (loopContext.isWorkProcessed)
			{
				sched_yield();
				return;
			}

			// Calculate maximum wait time
			long loopDelay = _CalculateLoopDelay(loopContext);
			if (loopDelay <= 0)
			{
				sched_yield();
				return;
			}

			timespec loopWait;
			loopWait.tv_sec = 0;
			loopWait.tv_nsec = loopDelay;
			timespec waitLimit = Utility::Threading::TimespecAdd(Utility::Threading::GetCurrentSystemTimespec(), loopWait);

			// Wait
			int loopCondLockResult = pthread_mutex_lock(&(m_pOwnerContext->QueueMutex()));
			if (loopCondLockResult == 0)
			{
				pthread_mutex_t *lockCopy = &(m_pOwnerContext->QueueMutex());
				auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
				FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

				if (!m_isShutdownRequested.load() && !m_isPauseRequested.load())
				{
					// Re-check if new entry added during processing
					//note: this is not strictly synchronized. for worst cases(maybe very rare), the job can get latency as the longest loop delay
					if (m_pOwnerContext->IsQueueEmptyRoughly())
					{
						pthread_cond_timedwait(&(m_pOwnerContext->QueueCondition()), &(m_pOwnerContext->QueueMutex()), &waitLimit);
					}
				}
			}
			else
			{
				// If lock fails, just wait 1ms and go
				Utility::Threading::PthreadSleepMS(1);
			}
		}

		//------------------------------------------------------------------------------
		long ThreadWorker::_CalculateLoopDelay(_Internal::LoopProcessContext &loopContext)
		{
			if (loopContext.noWorkCount <= 1)
			{
				return 0;
			}

			long delay = 0;
			switch (loopContext.noWorkCount)
			{
				case 2: delay = 2 * 1000000; break;	// 2ms
				case 3: delay = 20 * 1000000; break;	// 20ms

				case 4:
				default:
					{
						// 200ms and capping
						delay = 200 * 1000000;
						loopContext.noWorkCount = 4;
					}
					break;
			}

			return delay;
		}

		//------------------------------------------------------------------------------
		void ThreadWorker::_WaitPause()
		{
			// Loop for deal with spurious wakeup
			while (m_isPauseRequested.load())
			{
				int pauseCondLockResult = pthread_mutex_lock(&(m_pOwnerContext->PauseMutex()));
				if (pauseCondLockResult == 0)
				{
					pthread_mutex_t *lockCopy = &(m_pOwnerContext->PauseMutex());
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					if (m_isPauseRequested.load())
					{
						m_isThreadPaused.store(true);
						pthread_cond_wait(&(m_pOwnerContext->PauseCondition()), &(m_pOwnerContext->PauseMutex()));
					}
				}
				else
				{
					// Sleep 1ms and re-check
					Utility::Threading::PthreadSleepMS(1);
				}

				// Thread Finish
				if (m_isShutdownRequested.load())
				{
					m_isPauseRequested.store(false);
					break;
				}
			}
			m_isThreadPaused.store(false);
		}

		//------------------------------------------------------------------------------
		void *ThreadWorker::_ThreadStartHelper(void *threadThis)
		{
			((ThreadWorker *)threadThis)->_ThreadFunction();
			return nullptr;
		}
	}
}
