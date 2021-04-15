////////////////////////////////////////////////////////////////////////////////
// AsyncTaskHttpComponent_iOS.mm (Leggiero/Modules - HTTP)
//
// Async HTTP Processor Implementation for iOS
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsyncTaskHttpComponent.h"

// Leggiero.HTTP
#include "Async/AsyncHttp_iOS.h"


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
				return iOS::StartHTTPRequestAsync(url, method, parameters, connectTimeout);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<HTTPDownloadTask> AsyncTaskHttpComponent::Download(const std::string &downloadFilePath, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, bool isBackgroundTask, long connectTimeout)
			{
				return iOS::StartHTTPDownloadAsync(downloadFilePath, isBackgroundTask, url, method, parameters, connectTimeout);
			}
		}
	}
}
