////////////////////////////////////////////////////////////////////////////////
// HttpUtility.cpp (Leggiero/Modules - HTTP)
//
// HTTP Common Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HttpUtility.h"

// Standard Library
#include <cctype>

// Leggiero.Utility
#include <Utility/Encoding/URLEncoding.h>


namespace Leggiero
{
	namespace HTTP
	{
		//------------------------------------------------------------------------------
		bool IsWithScheme(const std::string &url)
		{
			size_t delimiterPos = url.find(':');
			if (delimiterPos == std::string::npos)
			{
				return false;
			}
			if (delimiterPos == 0)
			{
				// null scheme?
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------
		bool IsHTTPSecureProtocol(const std::string &url)
		{
			constexpr size_t kHTTPSSchemeLength = 6;

			if (url.length() < kHTTPSSchemeLength)
			{
				return false;
			}

			const char kHTTPSProtocolScheme[] = "https:";
			for (int i = 0; i < kHTTPSSchemeLength; ++i)
			{
				if (tolower(url[i]) != kHTTPSProtocolScheme[i])
				{
					return false;
				}
			}

			return true;
		}

		//------------------------------------------------------------------------------
		std::string EncodePOSTParameters(const POSTParameterMap &parameters)
		{
			std::vector<char> resultBuffer;
			std::string encodeBuffer;

			// To avoid reallocation
			size_t approxSize = 0;
			for (POSTParameterMap::const_iterator it = parameters.cbegin(); it != parameters.cend(); ++it)
			{
				approxSize += 2;	// for &, =, ...
				approxSize += it->first.length();
				approxSize += it->second.length();
			}
			approxSize = approxSize * 3 / 2;	// Rough margin for URL Encoding
			resultBuffer.reserve(approxSize);

			bool isFirst = true;
			for (POSTParameterMap::const_iterator it = parameters.cbegin(); it != parameters.cend(); ++it)
			{
				if (isFirst)
				{
					isFirst = false;
				}
				else
				{
					resultBuffer.push_back('&');
				}

				encodeBuffer = Utility::Encoding::URL::Encode(it->first, true);
				resultBuffer.insert(resultBuffer.end(), encodeBuffer.begin(), encodeBuffer.end());

				resultBuffer.push_back('=');

				encodeBuffer = Utility::Encoding::URL::Encode(it->second, true);
				resultBuffer.insert(resultBuffer.end(), encodeBuffer.begin(), encodeBuffer.end());
			}

			return std::string(&resultBuffer[0], resultBuffer.size());
		}


		namespace Settings
		{
			namespace _Internal
			{
				//------------------------------------------------------------------------------
				static long s_defaultRequestTimeout = 10;
			}

			//------------------------------------------------------------------------------
			long GetHTTPRequestDefaultTimeoutInSec()
			{
				return _Internal::s_defaultRequestTimeout;
			}

			//------------------------------------------------------------------------------
			void SetHTTPRequestDefaultTimeoutInSec(long timeout)
			{
				_Internal::s_defaultRequestTimeout = timeout;
			}
		}
	}
}
