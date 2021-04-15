////////////////////////////////////////////////////////////////////////////////
// Async/AsyncHttpRequestTask.h (Leggiero/Modules - HTTP)
//
// Asynchronous HTTP Request Task
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__ASYNC__ASYNC_HTTP_REQUEST_TASK_H
#define __LM_HTTP__ASYNC__ASYNC_HTTP_REQUEST_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Task
#include <Task/Tasks/ValueTasks.h>

// Leggiero.HTTP
#include "../HttpCommonType.h"
#include "../HttpResult.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			// Task to execute asynchronous HTTP request
			class HTTPRequestTask
				: public Task::IAsyncValueTask<std::shared_ptr<HTTPResponseData> >
			{
			public:
				HTTPRequestTask();
				virtual ~HTTPRequestTask() { }
			};
		}
	}
}

#endif
