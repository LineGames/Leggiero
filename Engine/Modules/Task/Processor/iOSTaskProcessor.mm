////////////////////////////////////////////////////////////////////////////////
// Processor/iOSTaskProcessor.mm (Leggiero/Modules - Task)
//
// iOS Platform General Task Processor Implementation using GCD
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSTaskProcessor.h"

// System Library
#import <Foundation/Foundation.h>

// Leggiero.Task
#include "../_Internal/_TaskExecutionEntry.h"
#include "../_Internal/ITaskManagerSystemFunctions.h"
#include "../Tasks/ITask.h"


namespace Leggiero
{
	namespace Task
	{
		namespace iOS
		{
			//////////////////////////////////////////////////////////////////////////////// IOSTaskProcessor
			
			//------------------------------------------------------------------------------
			IOSTaskProcessor::IOSTaskProcessor(ITaskManagerSystemFunctions *pManager)
				: m_pManager(pManager)
				, m_myPointerHolder(std::make_shared<Utility::Object::PointerHolder>(this))
			{
			}
			
			//------------------------------------------------------------------------------
			IOSTaskProcessor::~IOSTaskProcessor()
			{
				m_myPointerHolder->NotifyTargetDeleted();
			}
			
			//------------------------------------------------------------------------------
			// Give job to processor
			void IOSTaskProcessor::GiveJob(TaskExecutionEntry *job)
			{
				if (job == nullptr)
				{
					return;
				}
				
				ITask *jobTask = job->task.get();
				if (jobTask == nullptr)
				{
					m_pManager->ReleaseExecution(job);
					return;
				}
				if (jobTask->IsFinished())
				{
					m_pManager->ReleaseExecution(job);
					return;
				}
				if (jobTask->HasError())
				{
					jobTask->State().store(TaskState::kError);
					m_pManager->ReleaseExecution(job);
					return;
				}
				
				__block dispatch_queue_t processingQueue;
				
				if (jobTask->GetTaskPriority() == TaskPriorityClass::kBackground)
				{
					processingQueue = dispatch_get_global_queue(QOS_CLASS_BACKGROUND, 0);
				}
				else
				{
					processingQueue = dispatch_get_main_queue();
				}
				
				void (^taskProcessingBlock)(void);
				__block void (^taskProcessingBlockCopy)(void);
				std::weak_ptr<Utility::Object::PointerHolder> processorHolder;
				taskProcessingBlockCopy = taskProcessingBlock = ^(void){
					// Lock Processor
					std::shared_ptr<Utility::Object::PointerHolder> ownerProcessorHolder(processorHolder.lock());
					IOSTaskProcessor *processor = nullptr;
					std::unique_ptr<Utility::Object::PointerHolder::IHoldContext> lockContext;
					if (ownerProcessorHolder)
					{
						lockContext = ownerProcessorHolder->LockTarget();
						if (lockContext)
						{
							processor = lockContext->GetTypedObject<IOSTaskProcessor>();
						}
					}
					
					if (processor == nullptr)
					{
						// Not valid context
						if (job->task)
						{
							if (!job->task->IsFinished())
							{
								job->task->State().store(TaskState::kError);
							}
						}
						return;
					}
					
					// Check Task
					ITask *pTask = job->task.get();
					if (pTask == nullptr)
					{
						processor->m_pManager->ReleaseExecution(job);
						return;
					}
					if (pTask->IsFinished())
					{
						processor->m_pManager->ReleaseExecution(job);
						return;
					}
					if (pTask->HasError())
					{
						pTask->State().store(TaskState::kError);
						processor->m_pManager->ReleaseExecution(job);
						return;
					}

					// Before Start Process
					if (!Leggiero::Utility::SyntacticSugar::HasFlag(pTask->GetTaskState(), TaskState::kJobStarted))
					{
						pTask->OnBeforeProcess();
						pTask->State().store(pTask->GetTaskState() | TaskState::kJobStarted);
					}
					if (pTask->HasError())
					{
						pTask->State().store(TaskState::kError);
						processor->m_pManager->ReleaseExecution(job);
						return;
					}

					// Do Main Task
					pTask->State().store(pTask->GetTaskState() | TaskState::kJobProcessing);
					
					pTask->OnBeforeStepProcess();

					TaskDoneResult taskResult = pTask->Do();
					job->lastStep = SchedulingClock::now();

					pTask->OnAfterStepProcess();

					pTask->State().store(pTask->GetTaskState() & (~TaskState::kJobProcessing));
					if (pTask->HasError())
					{
						pTask->State().store(TaskState::kError);
						processor->m_pManager->ReleaseExecution(job);
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
									processor->m_pManager->ReleaseExecution(job);
									return;
								}
								else
								{
									// Finished
									pTask->State().store(TaskState::kDone);
									processor->m_pManager->ReleaseExecution(job);
								}
							}
							break;

						case TaskDoneResult::ResultType::kYield:
							{
								dispatch_async(processingQueue, taskProcessingBlockCopy);
							}
							break;

						case TaskDoneResult::ResultType::kSleep:
							{
								std::chrono::duration<int64_t, std::nano> nanoSleep(std::chrono::duration_cast<std::chrono::duration<int64_t, std::nano> >(taskResult.duration));
								dispatch_time_t afterTime = dispatch_time(DISPATCH_TIME_NOW, nanoSleep.count());
								dispatch_after(afterTime, processingQueue, taskProcessingBlockCopy);
							}
							break;
							
						case TaskDoneResult::ResultType::kWaitCondition:
							{
								if (pTask->IsTaskReady())
								{
									dispatch_async(processingQueue, taskProcessingBlockCopy);
								}
								else
								{
									processor->m_pManager->RequestExecution(job);
								}
							}
							break;
					}
				};
				
				dispatch_async(processingQueue, taskProcessingBlock);
			}
			
			//------------------------------------------------------------------------------
			void IOSTaskProcessor::PrepareProcessorShutdown()
			{
				m_myPointerHolder->NotifyTargetDeleted();
			}
		}
	}
}
