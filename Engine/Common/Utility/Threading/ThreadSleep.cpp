////////////////////////////////////////////////////////////////////////////////
// Threading/ThreadSleep.cpp (Leggiero - Utility)
//
// Common Implementation of Thread Sleep Utilites
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ThreadSleep.h"

// Standard Library
#include <chrono>


namespace Leggiero
{
	namespace Utility
	{
		namespace Threading
		{
			//------------------------------------------------------------------------------
			// Sleep pthread context given microseconds period
			// The period should not equal or exceed 1 second.
			int PthreadSleepuS(long microseconds)
			{
				return PthreadSleep(microseconds * 1000);
			}

			//------------------------------------------------------------------------------
			// Sleep pthread context given milliseconds period
			// The period should not equal or exceed 1 second.
			int PthreadSleepMS(long milliseconds)
			{
				return PthreadSleep(milliseconds * 1000000);
			}

			//------------------------------------------------------------------------------
			// Get system time as timespec from epoch
			timespec GetCurrentSystemTimespec()
			{
				std::chrono::nanoseconds durationInNS(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()));

				timespec resultValue;
				if (durationInNS.count() <= 0)
				{
					resultValue.tv_sec = 0;
					resultValue.tv_nsec = 0;
				}
				else
				{
					resultValue.tv_sec = static_cast<decltype(resultValue.tv_sec)>(durationInNS.count() / 1000000000L);
					resultValue.tv_nsec = static_cast<decltype(resultValue.tv_sec)>(durationInNS.count() % 1000000000L);
				}

				return resultValue;
			}
		}
	}
}
