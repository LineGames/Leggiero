////////////////////////////////////////////////////////////////////////////////
// Async/AsyncHttpDownloadTask.h (Leggiero/Modules - HTTP)
//
// Asynchronous HTTP File Download Task
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__ASYNC__ASYNC_HTTP_DOWNLOAD_TASK_H
#define __LM_HTTP__ASYNC__ASYNC_HTTP_DOWNLOAD_TASK_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

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
			class HTTPDownloadTask
				: public Task::IAsyncValueTask<HTTPRequestResult>
			{
			public:
				HTTPDownloadTask(Task::TaskPriorityClass priority = Task::TaskPriorityClass::kDefault);
				virtual ~HTTPDownloadTask() { }

			public:
				// Get estimated file size
				virtual size_t GetEstimatedFileSize() = 0;

				// Get currently downloaded data size
				virtual size_t GetDownloadedSize() = 0;

				// Get progress value. If total estimated size is unavailable, always return 0.0.
				// Result value may in range [0.0, 1.0], but can exceed the range because of invalid estimation of size. isClipped can prevent it.
				virtual float GetEstimatedProgress(bool isClipped = true) = 0;

				// Request download stop
				virtual void RequestToCancelDownload() = 0;

				// Check whether if the downloading finished by force before end of download.
				virtual bool IsCanceledDownload() = 0;
			};
		}
	}
}

#endif
