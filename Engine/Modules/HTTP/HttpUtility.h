////////////////////////////////////////////////////////////////////////////////
// HttpUtility.h (Leggiero/Modules - HTTP)
//
// HTTP common utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__HTTP_UTILITY_H
#define __LM_HTTP__HTTP_UTILITY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <string>
#include <vector>

// Leggiero.HTTP
#include "HttpCommonType.h"


namespace Leggiero
{
	namespace HTTP
	{
		bool IsWithScheme(const std::string &url);
		bool IsHTTPSecureProtocol(const std::string &url);

		std::string EncodePOSTParameters(const POSTParameterMap &parameters);


		namespace Settings
		{
			long GetHTTPRequestDefaultTimeoutInSec();
			void SetHTTPRequestDefaultTimeoutInSec(long timeout);
		}
	}
}

#endif
