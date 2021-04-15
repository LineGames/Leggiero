////////////////////////////////////////////////////////////////////////////////
// HttpCommonType.h (Leggiero/Modules - HTTP)
//
// Common Type Definitions for HTTP Module
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__HTTP_COMMON_TYPE_H
#define __LM_HTTP__HTTP_COMMON_TYPE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <chrono>
#include <map>
#include <string>


namespace Leggiero
{
	namespace HTTP
	{
		// Supported HTTP Methods
		enum class RequestMethod
		{
			kGet,
			kPost,
		};


		// Key-Value Map for POST Request Parameter
		using POSTParameterMap = std::map<std::string, std::string>;

		// Empty POST Map
		extern const POSTParameterMap kEmptyPOSTParameterMap;


		// Clock type
		using HTTPSystemClock = std::chrono::system_clock;
	}
}

#endif
