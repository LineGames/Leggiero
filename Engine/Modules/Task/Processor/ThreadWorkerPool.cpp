////////////////////////////////////////////////////////////////////////////////
// Processor/ThreadWorkerPool.cpp (Leggiero/Modules - Task)
//
// Thread Worker Pool Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ThreadWorkerPool.h"

// Standard Library
#include <vector>

// Leggiero.Utility
#include <Utility/Threading/ThreadSleep.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventDispatcher.h>

// Leggiero.Task
#include "../_Internal/_TaskExecutionEntry.h"
#include "../_Internal/ITaskManagerSystemFunctions.h"
#include "../Tasks/ITask.h"
#include "ThreadWorker.h"


namespace Leggiero
{
	namespace Task
	{
		//////////////////////////////////////////////////////////////////////////////// ThreadWorkerPool

		//------------------------------------------------------------------------------
		ThreadWorkerPool::ThreadWorkerPool(ITaskManagerSystemFunctions *pManager, Application::ApplicationComponent *appComponent)
			: m_pManager(pManager), m_appComponentCopy(appComponent)
		{
			appComponent->SystemEventCenter().RegisterApplicationEventObserver(this);
		}

		//------------------------------------------------------------------------------
		ThreadWorkerPool::~ThreadWorkerPool()
		{
			m_appComponentCopy->SystemEventCenter().UnRegisterApplicationEventObserver(this);

			for (ThreadWorker *currentWorker : m_workerList)
			{
				if (currentWorker->IsRunning())
				{
					currentWorker->RequestStop();
				}
			}
			for (ThreadWorker *currentWorker : m_workerList)
			{
				delete currentWorker;
			}
			m_workerList.clear();

			TaskExecutionEntry *dequeuedJob = nullptr;
			while (m_jobs.try_dequeue(dequeuedJob))
			{
				if (dequeuedJob->task)
				{
					// Cancel Task
					ITask &task = *(dequeuedJob->task);
					if (!task.IsFinished())
					{
						task.SetErrorFlag();
						task.State().store(TaskState::kError);
					}
				}
				m_pManager->ReleaseExecution(dequeuedJob);
			}
		}

		//------------------------------------------------------------------------------
		int ThreadWorkerPool::GetWorkerCount()
		{
			auto lockContext = m_workerListLock.Lock();
			if (lockContext)
			{
				return static_cast<int>(m_workerList.size());
			}
			else
			{
				// Anyway, go
				return static_cast<int>(m_workerList.size());
			}
		}

		//------------------------------------------------------------------------------
		int ThreadWorkerPool::IncreaseWorker(int increaseCount)
		{
			if (increaseCount < 1)
			{
				return 0;
			}

			std::vector<ThreadWorker *> creatingBuffer;
			creatingBuffer.reserve(increaseCount);

			// Try max twice
			int maxTry = increaseCount * 2;
			for (int i = 0; i < maxTry; ++i)
			{
				ThreadWorker *createdWorker = _CreateWorker();
				if (createdWorker != nullptr)
				{
					creatingBuffer.push_back(createdWorker);
				}

				if (creatingBuffer.size() >= increaseCount)
				{
					break;
				}
			}

			if (creatingBuffer.empty())
			{
				return 0;
			}

			// Add to List - try 3-times
			bool isWorkersAdded = false;
			for (int i = 0; i < 3; ++i)
			{
				auto lockContext = m_workerListLock.Lock();
				if (lockContext)
				{
					m_workerList.insert(m_workerList.end(), creatingBuffer.begin(), creatingBuffer.end());
					isWorkersAdded = true;
					break;
				}
			}
			if (!isWorkersAdded)
			{
				// Something Wrong
				for (ThreadWorker *currentWorker : creatingBuffer)
				{
					currentWorker->RequestStop();
				}
				for (ThreadWorker *currentWorker : creatingBuffer)
				{
					delete currentWorker;
				}
				return 0;
			}

			return static_cast<int>(creatingBuffer.size());
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::DecreaseWorker()
		{
			auto lockContext = m_workerListLock.Lock();
			if (lockContext)
			{
				// Only do in lock context
				if (!m_workerList.empty())
				{
					ThreadWorker *removingWorker = m_workerList.front();
					m_workerList.pop_front();

					removingWorker->RequestStop();
					delete removingWorker;
				}
			}
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::GiveJob(TaskExecutionEntry *job)
		{
			m_jobs.enqueue(job);
			pthread_cond_signal(&m_queueCondition.GetConditionVariable());
		}

		//------------------------------------------------------------------------------
		ThreadWorker *ThreadWorkerPool::_CreateWorker()
		{
			ThreadWorker *createdWorker = new ThreadWorker(this);
			if (createdWorker == nullptr || !createdWorker->IsRunning())
			{
				delete createdWorker;
				return nullptr;
			}
			return createdWorker;
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::_CollectInvalidWorkers()
		{
			auto lockContext = m_workerListLock.Lock();
			if (lockContext)
			{
				for (std::list<ThreadWorker *>::iterator it = m_workerList.begin(); it != m_workerList.end(); )
				{
					if ((*it) == nullptr || !(*it)->IsRunning())
					{
						delete (*it);
						it = m_workerList.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::OnGoToBackground()
		{
			// Pause workers during app pause
			{
				auto lockContext = m_workerListLock.Lock();
				for (ThreadWorker *currentWorker : m_workerList)
				{
					if (currentWorker != nullptr)
					{
						currentWorker->RequestPause();
					}
				}
			}
			
			// Signal Workers to Pause Immediatly
			pthread_cond_broadcast(&m_queueCondition.GetConditionVariable());
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::OnReturnFromBackground()
		{
			{
				auto lockContext = m_workerListLock.Lock();
				for (ThreadWorker *currentWorker : m_workerList)
				{
					if (currentWorker != nullptr)
					{
						currentWorker->RequestResume();
					}
				}
			}

			pthread_cond_broadcast(&m_pauseCondition.GetConditionVariable());
		}


		//////////////////////////////////////////////////////////////////////////////// ThreadWorkerPool - IThreadWorkerContext

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::HandleDone(TaskExecutionEntry *execution)
		{
			m_pManager->ReleaseExecution(execution);
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::HandleYield(TaskExecutionEntry *execution)
		{
			m_jobs.enqueue(execution);
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::HandleWait(TaskExecutionEntry *execution, TaskDoneResult waitResult)
		{
			switch (waitResult.result)
			{
				case TaskDoneResult::ResultType::kSleep:
					{
						m_pManager->RequestDelayedExecution(execution, waitResult.duration);
					}
					break;

				case TaskDoneResult::ResultType::kWaitCondition:
					{
						m_pManager->RequestExecution(execution);
					}
					break;

				default:
					{
						// Not a wating execution
						if (execution->task)
						{
							ITask &task = *(execution->task);
							if (!task.IsFinished())
							{
								task.SetErrorFlag();
								task.State().store(TaskState::kError);
							}
						}
						m_pManager->ReleaseExecution(execution);
					}
					break;
			}
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::HandleError(TaskExecutionEntry *execution)
		{
			m_pManager->ReleaseExecution(execution);
		}

		//------------------------------------------------------------------------------
		void ThreadWorkerPool::ReleaseNotExecution(TaskExecutionEntry *execution)
		{
			m_pManager->ReleaseExecution(execution);
		}

		//------------------------------------------------------------------------------
		TaskExecutionEntry *ThreadWorkerPool::DequeueJob()
		{
			TaskExecutionEntry *dequeuedJob = nullptr;
			if (m_jobs.try_dequeue(dequeuedJob))
			{
				return dequeuedJob;
			}

			// Try Twice
			if (m_jobs.try_dequeue(dequeuedJob))
			{
				return dequeuedJob;
			}

			return nullptr;
		}
	}
}
