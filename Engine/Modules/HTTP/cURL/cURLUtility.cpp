////////////////////////////////////////////////////////////////////////////////
// cURL/cURLUtility.cpp (Leggiero/Modules - HTTP)
//
// cURL Common Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "cURLUtility.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace cURL
		{
			namespace Settings
			{
				//------------------------------------------------------------------------------
				namespace _Internal
				{
					static long kFollowHTTPRedirectionMax = 5L;
				}

				//------------------------------------------------------------------------------
				long GetFollowHTTPRedirectionMax()
				{
					return _Internal::kFollowHTTPRedirectionMax;
				}

				//------------------------------------------------------------------------------
				void SetFollowHTTPRedirectionMax(long maxRedirections)
				{
					_Internal::kFollowHTTPRedirectionMax = maxRedirections;
				}
			}

			//------------------------------------------------------------------------------
			void InitializeModuleCUrlSystem()
			{
				curl_global_init(CURL_GLOBAL_ALL);
			}

			//------------------------------------------------------------------------------
			void FinalizeModuleCUrlSystem()
			{
				curl_global_cleanup();
			}

			//------------------------------------------------------------------------------
			// Set cURL common options
			void SetCUrlCommonOptions(CURL *curl_handle)
			{
				long currentRedirectionMaxValue = Settings::_Internal::kFollowHTTPRedirectionMax;
				if (currentRedirectionMaxValue > 0)
				{
					curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
					curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, currentRedirectionMaxValue);
				}
				else
				{
					curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);
				}

				curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");

				curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
			}

			//------------------------------------------------------------------------------
			// Set cURL SSL Options
			void SetCUrlCommonSSLOptions(CURL *curl_handle)
			{
				curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
			}
		}
	}
}
