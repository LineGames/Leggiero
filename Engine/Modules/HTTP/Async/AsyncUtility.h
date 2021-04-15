////////////////////////////////////////////////////////////////////////////////
// Async/AsyncUtility.h (Leggiero/Modules - HTTP)
//
// Async Common Utility
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__ASYNC__ASYNC_UTILITY_H
#define __LM_HTTP__ASYNC__ASYNC_UTILITY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Task
#include <Task/TaskTypes.h>


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			namespace Settings
			{
				Task::TaskCapabilityType GetHTTPTaskCapability();
				void SetHTTPTaskCapability(Task::TaskCapabilityType capability);
			}
		}
	}
}

#endif
