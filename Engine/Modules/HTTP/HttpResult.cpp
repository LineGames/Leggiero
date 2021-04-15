////////////////////////////////////////////////////////////////////////////////
// HttpResult.cpp (Leggiero/Modules - HTTP)
//
// HTTP Result Classes Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HttpResult.h"

// Standard Library
#include <cstring>


namespace Leggiero
{
	namespace HTTP
	{
		//////////////////////////////////////////////////////////////////////////////// HTTPResponseData

		//------------------------------------------------------------------------------
		namespace _Internal
		{
			namespace HTTPResponseDataSettings
			{
				constexpr size_t kInitialBufferSize = 512;
			}
		}

		//------------------------------------------------------------------------------
		HTTPResponseData::HTTPResponseData()
			: m_currentDataSize(0)
		{
			m_dataBuffer = malloc(_Internal::HTTPResponseDataSettings::kInitialBufferSize);
			m_currentBufferSize = _Internal::HTTPResponseDataSettings::kInitialBufferSize;
		}

		//------------------------------------------------------------------------------
		HTTPResponseData::~HTTPResponseData()
		{
			free(m_dataBuffer);
		}

		//------------------------------------------------------------------------------
		size_t HTTPResponseData::_WriteMemoryCallbackForCURL(void *contents, size_t size, size_t nmemb, void *userp)
		{
			HTTPResponseData *pDataObject = (HTTPResponseData *)userp;

			if (pDataObject->m_currentDataSize == 0)
			{
				pDataObject->requestResult.responseStartTime = std::chrono::system_clock::now();
			}

			size_t realSize = size * nmemb;

			size_t resultSize = pDataObject->m_currentDataSize + realSize;
			if (resultSize > pDataObject->m_currentBufferSize)
			{
				size_t increasingBufferSize = pDataObject->m_currentBufferSize * 2;
				while (increasingBufferSize < resultSize)
				{
					increasingBufferSize *= 2;
				}
				pDataObject->m_dataBuffer = realloc(pDataObject->m_dataBuffer, increasingBufferSize);
				pDataObject->m_currentBufferSize = increasingBufferSize;
			}

			memcpy((char *)pDataObject->m_dataBuffer + pDataObject->m_currentDataSize, contents, realSize);
			pDataObject->m_currentDataSize += realSize;

			return realSize;
		}
	}
}
