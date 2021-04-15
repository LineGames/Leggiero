////////////////////////////////////////////////////////////////////////////////
// TaskTypes.h (Leggiero/Modules - Task)
//
// Type Definitions for Leggiero Task System
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_TASK__TASK_TYPES_H
#define __LM_TASK__TASK_TYPES_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <chrono>
#include <cstdint>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>


namespace Leggiero
{
	namespace Task
	{
		using SchedulingClock = std::chrono::system_clock;


		// Result for Task Doing Step
		struct TaskDoneResult
		{
		public:
			enum class ResultType
			{
				kFinished = 0, 

				kYield = 1, 
				kSleep = 2, 
				kWaitCondition = 3, 
			};

		public:
			ResultType					result;
			SchedulingClock::duration	duration;

			TaskDoneResult(ResultType res, SchedulingClock::duration dur = SchedulingClock::duration::zero())
				: result(res), duration(dur)
			{
				if (res == ResultType::kSleep)
				{
					if (dur <= SchedulingClock::duration::zero())
					{
						// Consider as yield
						result = ResultType::kYield;
					}
				}
			}
		};


		// Task State
		enum class TaskState : uint32_t
		{
			kNone = 0x0, 

			kHasError = 0x1,

			kJobQueued = 0x10,
			kJobStarted = 0x20,
			kJobProcessing = 0x40,
			kJobFinished = 0x80,

			kWaiting = kJobQueued,
			kProcessing = (kJobQueued | kJobStarted | kJobProcessing),
			kContinueWaiting = (kJobQueued | kJobStarted),
			kDone = (kJobStarted | kJobFinished),
			kError = (kJobStarted | kJobFinished | kHasError),
		};
		MAKE_ENUM_FLAG(TaskState);


		// Task Priority Class
		enum class TaskPriorityClass
		{
			kDefault = 0, 
			kBackground = 1, 
			kHighPriority = 2, 
		};


		// Task Properties
		enum class TaskPropertyType
		{
			kNone = 0, 
		};
		MAKE_ENUM_FLAG(TaskPropertyType);


		// Task Capabilty
		//note: not defined as enum class for user defined capabilty extension
		using TaskCapabilityType = uint32_t;

		namespace TaskCapabilities
		{
			// General tasks with no speical requirements
			constexpr TaskCapabilityType kGeneral = static_cast<TaskCapabilityType>(0);

			// Tasks need Graphics Context
			constexpr TaskCapabilityType kGraphics = static_cast<TaskCapabilityType>(0x1);
		}
	}
}

#endif
