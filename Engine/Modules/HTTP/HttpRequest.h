////////////////////////////////////////////////////////////////////////////////
// HttpRequest.h (Leggiero/Modules - HTTP)
//
// HTTP Request Function Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__HTTP_REQUEST_H
#define __LM_HTTP__HTTP_REQUEST_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.HTTP
#include "HttpCommonType.h"
#include "HttpUtility.h"
#include "HttpResult.h"


namespace Leggiero
{
	namespace HTTP
	{
		// Do HTTP Request
		std::shared_ptr<HTTPResponseData> DoHTTPRequest(const std::string &url, RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, 
			long connectTimeout = Settings::GetHTTPRequestDefaultTimeoutInSec());

		// Do HTTP Request and save response to a file in the given path
		HTTPRequestResult DoHTTPDownload(const std::string &downloadFilePath, const std::string &url, RequestMethod method = RequestMethod::kGet, const POSTParameterMap &parameters = kEmptyPOSTParameterMap, 
			long connectTimeout = Settings::GetHTTPRequestDefaultTimeoutInSec());
	}
}

#endif
