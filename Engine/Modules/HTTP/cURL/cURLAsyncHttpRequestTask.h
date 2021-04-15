////////////////////////////////////////////////////////////////////////////////
// cURL/cURLAsyncHttpRequestTask.h (Leggiero/Modules - HTTP)
//
// libcurl and TaskManager based Async HTTP Request Task
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__CURL__CURL_ASYNC_HTTP_REQUEST_TASK_H
#define __LM_HTTP__CURL__CURL_ASYNC_HTTP_REQUEST_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.HTTP
#include "../Async/AsyncHttpRequestTask.h"
#include "../HttpUtility.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Task
	{
		class TaskManagerComponent;
	}


	namespace HTTP
	{
		namespace cURL
		{
			// libcurl based Async HTTP Request Task
			class CURLHTTPRequestTask
				: public Async::HTTPRequestTask
			{
			public:
				CURLHTTPRequestTask(const std::string &url, RequestMethod method, const POSTParameterMap &parameters, int connectTimeout);
				virtual ~CURLHTTPRequestTask();

			public:	// ITask
				// Do Real Task Works
				virtual Task::TaskDoneResult Do() override;

			public:	// IAsyncValueTask
				// Check whether the task have result value
				virtual bool HasValue() const override { return (bool)m_result; }

				// Get result value
				virtual std::shared_ptr<HTTPResponseData> GetValue() override { return m_result; }

			protected:
				const std::string		m_requestURL;
				const RequestMethod		m_requestMethod;
				const POSTParameterMap	m_requestParameters;

				const int m_optionConnectTimeout;

				std::shared_ptr<HTTPResponseData> m_result;

			public:
				static std::shared_ptr<CURLHTTPRequestTask> StartHTTPRequestAsync(Task::TaskManagerComponent *taskManager, const std::string &url, RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap,
					long connectTimeout = HTTP::Settings::GetHTTPRequestDefaultTimeoutInSec());
			};
		}
	}
}

#endif
