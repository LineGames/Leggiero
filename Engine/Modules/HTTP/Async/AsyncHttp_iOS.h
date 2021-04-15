////////////////////////////////////////////////////////////////////////////////
// Async/AsyncHttp_iOS.h (Leggiero/Modules - HTTP)
//
// Asynchronous HTTP functions for iOS platform
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__ASYNC__ASYNC_HTTP__IOS_H
#define __LM_HTTP__ASYNC__ASYNC_HTTP__IOS_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.HTTP
#include "../HttpUtility.h"
#include "AsyncHttpRequestTask.h"
#include "AsyncHttpDownloadTask.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			namespace iOS
			{
				std::shared_ptr<HTTPRequestTask> StartHTTPRequestAsync(const std::string &url, RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, long connectTimeout = HTTP::Settings::GetHTTPRequestDefaultTimeoutInSec());
				
				std::shared_ptr<HTTPDownloadTask> StartHTTPDownloadAsync(const std::string &downloadFilePath, bool isBackgroundTask, const std::string &url, RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, long connectTimeout = HTTP::Settings::GetHTTPRequestDefaultTimeoutInSec());
			}
		}
	}
}

#endif
