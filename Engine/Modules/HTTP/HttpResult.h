////////////////////////////////////////////////////////////////////////////////
// HttpResult.h (Leggiero/Modules - HTTP)
//
// HTTP request result type definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_HTTP__HTTP_RESULT_H
#define __LM_HTTP__HTTP_RESULT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.HTTP
#include "HttpCommonType.h"


namespace Leggiero
{
	namespace HTTP
	{
		// Structure to store HTTP request result
		struct HTTPRequestResult
		{
		public:
			bool		isRequestSuccess;
			int			statusCode;
			std::string errorString;

		public:
			HTTPSystemClock::time_point requestStartTime;
			HTTPSystemClock::time_point responseStartTime;
			HTTPSystemClock::time_point downloadFinishTime;
		};


		// HTTP Request Result with Reponse Data
		class HTTPResponseData
			: private Utility::SyntacticSugar::NonCopyable
		{
			friend std::shared_ptr<HTTPResponseData> DoHTTPRequest(const std::string &, RequestMethod, const POSTParameterMap &, long);

		public:
			HTTPRequestResult requestResult;

		public:
			const void *GetResultBufferPtr() const { return m_dataBuffer; }
			size_t GetResultDataSize() const { return m_currentDataSize; }

			std::string GetResultString() const { return std::string((const char *)m_dataBuffer, m_currentDataSize); }

		public:
			HTTPResponseData();
			virtual ~HTTPResponseData();

		protected:
			static size_t _WriteMemoryCallbackForCURL(void *contents, size_t size, size_t nmemb, void *userp);

		protected:
			void	*m_dataBuffer;
			size_t	m_currentDataSize;
			size_t	m_currentBufferSize;
		};
	}
}

#endif
