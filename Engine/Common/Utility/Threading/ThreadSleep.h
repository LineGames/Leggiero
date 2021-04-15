////////////////////////////////////////////////////////////////////////////////
// Threading/ThreadSleep.h (Leggiero - Utility)
//
// Thread Sleep Utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__THREADING__THREAD_SLEEP_H
#define __UTILITY__THREADING__THREAD_SLEEP_H


// External Library
#include <pthread.h>


namespace Leggiero
{
	namespace Utility
	{
		namespace Threading
		{
			// Sleep pthread context given nanoseconds period
			// The period should not be equal or exceed 1 second.
			int PthreadSleep(long nanoseconds);

			// Sleep pthread context given microseconds period
			// The period should not equal or exceed 1 second.
			int PthreadSleepuS(long microseconds);

			// Sleep pthread context given milliseconds period
			// The period should not equal or exceed 1 second.
			int PthreadSleepMS(long milliseconds);


			// Get system time as timespec from epoch
			timespec GetCurrentSystemTimespec();

			// Add two duration represented by timespec struct
			inline timespec TimespecAdd(const timespec &t1, const timespec &t2)
			{
				timespec result;
				result.tv_sec = t1.tv_sec + t2.tv_sec;
				result.tv_nsec = t1.tv_nsec + t2.tv_nsec;
				if (result.tv_nsec > 1000000000L)
				{
					result.tv_sec += result.tv_nsec / 1000000000L;
					result.tv_nsec = result.tv_nsec % 1000000000L;
				}

				return result;
			}
		}
	}
}

#endif
