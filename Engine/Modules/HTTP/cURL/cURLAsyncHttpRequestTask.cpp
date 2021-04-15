////////////////////////////////////////////////////////////////////////////////
// cURL/cURLAsyncHttpRequestTask.cpp (Leggiero/Modules - HTTP)
//
// cURL Async HTTP Request Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "cURLAsyncHttpRequestTask.h"

// Leggiero.Task
#include <Task/TaskManagerComponent.h>

// Leggiero.HTTP
#include "../HttpRequest.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace cURL
		{
			//////////////////////////////////////////////////////////////////////////////// CURLHTTPRequestTask

			//------------------------------------------------------------------------------
			CURLHTTPRequestTask::CURLHTTPRequestTask(const std::string &url, RequestMethod method, const POSTParameterMap &parameters, int connectTimeout)
				: m_requestURL(url), m_requestMethod(method), m_requestParameters(parameters), m_optionConnectTimeout(connectTimeout)
			{
			}

			//------------------------------------------------------------------------------
			CURLHTTPRequestTask::~CURLHTTPRequestTask()
			{
			}

			//------------------------------------------------------------------------------
			Task::TaskDoneResult CURLHTTPRequestTask::Do()
			{
				m_result = DoHTTPRequest(m_requestURL, m_requestMethod, m_requestParameters, m_optionConnectTimeout);
				return Task::TaskDoneResult(Task::TaskDoneResult::ResultType::kFinished);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<CURLHTTPRequestTask> CURLHTTPRequestTask::StartHTTPRequestAsync(Task::TaskManagerComponent *taskManager, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
			{
				std::shared_ptr<CURLHTTPRequestTask> requestTask(std::make_shared<CURLHTTPRequestTask>(url, method, parameters, connectTimeout));
				
				if (!taskManager->ExecuteTask(requestTask))
				{
					// Something Wrong
					return nullptr;
				}

				return requestTask;
			}
		}
	}
}
