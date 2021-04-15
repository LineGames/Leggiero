////////////////////////////////////////////////////////////////////////////////
// Threading/ThreadSleep_WinPC.cpp (Leggiero - Utility)
//
// Windows PC Platform Implementation of Thread Sleep Utilites
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ThreadSleep.h"


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

				return pthread_delay_np(&sleepTime);
			}
		}
	}
}
