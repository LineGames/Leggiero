////////////////////////////////////////////////////////////////////////////////
// cURL/cURLAsyncHttpDownloadTask.h (Leggiero/Modules - HTTP)
//
// libcurl and TaskManager based Async HTTP File Download Task
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__CURL__CURL_ASYNC_HTTP_DOWNLOAD_TASK_H
#define __LM_HTTP__CURL__CURL_ASYNC_HTTP_DOWNLOAD_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>

// Leggiero.HTTP
#include "../Async/AsyncHttpDownloadTask.h"
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
			// Forward Declaration
			namespace _Internal
			{
				class WritingState;
			}


			// libcurl based Async HTTP Download Task
			class CURLHTTPDownloadTask
				: public Async::HTTPDownloadTask
			{
				friend class _Internal::WritingState;

			public:
				CURLHTTPDownloadTask(const std::string &downloadFilePath, bool isBackgroundTask, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, int connectTimeout);
				virtual ~CURLHTTPDownloadTask();

			public:	// ITask
				// Do Real Task Works
				virtual Task::TaskDoneResult Do() override;

			public:	// IAsyncValueTask
				// Check whether the task have result value
				virtual bool HasValue() const override
				{
					Task::TaskState taskState = this->GetTaskState();
					return (Utility::SyntacticSugar::HasFlag(taskState, Task::TaskState::kJobFinished)
						&& !Utility::SyntacticSugar::HasFlag(taskState, Task::TaskState::kHasError));
				}

				// Get result value
				virtual HTTPRequestResult GetValue() override { return m_result; }

			public:	// Async::HTTPDownloadTask
				// Get estimated file size. 0 if unavailable.
				virtual size_t GetEstimatedFileSize() override { return m_estimatedDownloadSize; }

				// Get currently downloaded data size
				virtual size_t GetDownloadedSize() override { return m_downloadedSize; }

				// Get progress value. If total estimated size is unavailable, always return 0.0.
				// Result value may in range [0.0, 1.0], but can exceed the range because of invalid estimation of size. isClipped can prevent it.
				virtual float GetEstimatedProgress(bool isClipped = true) override;

				// Request download stop
				virtual void RequestToCancelDownload() override { m_isCancelRequested.store(true); }

				// Check whether if the downloading finished by force before end of download.
				virtual bool IsCanceledDownload() override { return m_isCanceled.load(); }

			protected:
				const std::string		m_downloadFilePath;
				const std::string		m_requestURL;
				const RequestMethod		m_requestMethod;
				const POSTParameterMap	m_requestParameters;

				const int m_optionConnectTimeout;

				size_t m_estimatedDownloadSize;
				size_t m_downloadedSize;

				std::atomic_bool	m_isCancelRequested;

				HTTPRequestResult	m_result;
				std::atomic_bool	m_isCanceled;

			public:
				static std::shared_ptr<CURLHTTPDownloadTask> StartHTTPDownloadAsync(Task::TaskManagerComponent *taskManager, const std::string &downloadFilePath, bool isBackgroundTask, const std::string &url,
					RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, long connectTimeout = HTTP::Settings::GetHTTPRequestDefaultTimeoutInSec());
			};
		}
	}
}

#endif
