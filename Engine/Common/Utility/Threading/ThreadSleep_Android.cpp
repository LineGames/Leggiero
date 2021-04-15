////////////////////////////////////////////////////////////////////////////////
// Threading/ThreadSleep_Android.cpp (Leggiero - Utility)
//
// Android Platform Implementation of Thread Sleep Utilites
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
			// Sleep pthread context given nanoseconds period
			// The period should not be equal or exceed 1 second.
			int PthreadSleep(long nanoseconds)
			{
				if (nanoseconds < 0 || nanoseconds > 999999999)
				{
					return 1;
				}

				timespec sleepTime;
				sleepTime.tv_sec = 0;
				sleepTime.tv_nsec = nanoseconds;

				return nanosleep(&sleepTime, NULL);
			}
		}
	}
}
