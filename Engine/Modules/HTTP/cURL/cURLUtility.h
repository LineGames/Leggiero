////////////////////////////////////////////////////////////////////////////////
// cURL/cURLUtility.h (Leggiero/Modules - HTTP)
//
// libcurl common utilites
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__CURL__CURL_UTILITY_H
#define __LM_HTTP__CURL__CURL_UTILITY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <curl/curl.h>

// Leggiero.HTTP
#include "../HttpCommonType.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace cURL
		{
			void InitializeModuleCUrlSystem();
			void FinalizeModuleCUrlSystem();

			// Set cURL common options
			void SetCUrlCommonOptions(CURL *curl_handle);

			// Set cURL SSL Options
			void SetCUrlCommonSSLOptions(CURL *curl_handle);


			namespace Settings
			{
				long GetFollowHTTPRedirectionMax();
				void SetFollowHTTPRedirectionMax(long maxRedirections);
			}
		}
	}
}

#endif
