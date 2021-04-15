////////////////////////////////////////////////////////////////////////////////
// cURL/cURLAsyncTaskHttpComponent.cpp (Leggiero/Modules - HTTP)
//
// Async HTTP Processor Implementation using cURL and TaskManager
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "../AsyncTaskHttpComponent.h"

// Leggiero.Task
#include <Task/TaskManagerComponent.h>

// Leggiero.HTTP
#include "cURLAsyncHttpRequestTask.h"
#include "cURLAsyncHttpDownloadTask.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			//////////////////////////////////////////////////////////////////////////////// AsyncTaskHttpComponent

			//------------------------------------------------------------------------------
			std::shared_ptr<HTTPRequestTask> AsyncTaskHttpComponent::Request(const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
			{
				return cURL::CURLHTTPRequestTask::StartHTTPRequestAsync(m_taskManager, url, method, parameters, connectTimeout);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<HTTPDownloadTask> AsyncTaskHttpComponent::Download(const std::string &downloadFilePath, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, bool isBackgroundTask, long connectTimeout)
			{
				return cURL::CURLHTTPDownloadTask::StartHTTPDownloadAsync(m_taskManager, downloadFilePath, isBackgroundTask, url, method, parameters, connectTimeout);
			}
		}
	}
}
