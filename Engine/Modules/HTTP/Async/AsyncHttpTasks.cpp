////////////////////////////////////////////////////////////////////////////////
// Async/AsyncHttpTasks.cpp (Leggiero/Modules - HTTP)
//
// Basic implementation of async HTTP tasks
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsyncHttpRequestTask.h"
#include "AsyncHttpDownloadTask.h"

// Leggiero.HTTP
#include "AsyncUtility.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			//////////////////////////////////////////////////////////////////////////////// HTTPRequestTask

			//------------------------------------------------------------------------------
			HTTPRequestTask::HTTPRequestTask()
				: Task::IAsyncValueTask<std::shared_ptr<HTTPResponseData> >(Settings::GetHTTPTaskCapability())
			{
			}


			//////////////////////////////////////////////////////////////////////////////// HTTPDownloadTask

			//------------------------------------------------------------------------------
			HTTPDownloadTask::HTTPDownloadTask(Task::TaskPriorityClass priority)
				: Task::IAsyncValueTask<HTTPRequestResult>(Settings::GetHTTPTaskCapability(), priority)
			{
			}
		}
	}
}
