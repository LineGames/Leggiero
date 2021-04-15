////////////////////////////////////////////////////////////////////////////////
// cURL/cURLHttpRequest.cpp (Leggiero/Modules - HTTP)
//
// cURL based HTTP Request Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "../HttpRequest.h"

// Standard Library
#include <cstdio>
#include <cstdlib>

// External Library
#include <curl/curl.h>
#include <fmt/format.h>

// Leggiero.HTTP
#include "cURLUtility.h"


namespace Leggiero
{
	namespace HTTP
	{
		//------------------------------------------------------------------------------
		// Do HTTP Request
		std::shared_ptr<HTTPResponseData> DoHTTPRequest(const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
		{
			std::shared_ptr<HTTPResponseData> responseData(std::make_shared<HTTPResponseData>());

			// Initialize Default curl
			struct curl_slist *headerList = NULL;
			CURL *curl_handle = curl_easy_init();

			curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, HTTPResponseData::_WriteMemoryCallbackForCURL);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)(responseData.get()));

			// Options
			cURL::SetCUrlCommonOptions(curl_handle);
			curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, connectTimeout);

			// Process SSL
			bool isSSL = IsHTTPSecureProtocol(url);

			if (isSSL)
			{
				cURL::SetCUrlCommonSSLOptions(curl_handle);
			}

			// Process Parameters
			std::string encodedParameters;
			if (parameters.size() > 0)
			{
				encodedParameters = EncodePOSTParameters(parameters);

				curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long)encodedParameters.length());
				curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, encodedParameters.c_str());
			}

			// Set Method
			switch (method)
			{
				case RequestMethod::kGet:
					curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
					break;

				case RequestMethod::kPost:
					curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
					curl_easy_setopt(curl_handle, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
					headerList = curl_slist_append(headerList, "Expect:");
					break;
			}

			// Apply Header
			if (headerList != NULL)
			{
				curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerList);
			}

			// Do Request
			responseData->requestResult.requestStartTime = HTTPSystemClock::now();
			CURLcode res = curl_easy_perform(curl_handle);

			if (res == CURLE_OK)
			{
				responseData->requestResult.isRequestSuccess = true;

				long http_code = 0;
				curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
				responseData->requestResult.statusCode = (int)http_code;
			}
			else
			{
				responseData->requestResult.isRequestSuccess = false;
				responseData->requestResult.statusCode = 0;
				responseData->requestResult.errorString = curl_easy_strerror(res);
			}
			responseData->requestResult.downloadFinishTime = HTTPSystemClock::now();

			// Clean Up
			curl_easy_cleanup(curl_handle);

			if (headerList != NULL)
			{
				curl_slist_free_all(headerList);
			}

			return responseData;
		}

		//------------------------------------------------------------------------------
		namespace _Internal
		{
			struct WritingState
			{
				FILE				*pStream;
				size_t				writtenBytes;
				HTTPRequestResult	*pResult;

			public:
				WritingState(FILE *stream, HTTPRequestResult *result)
					: writtenBytes(0)
					, pStream(stream), pResult(result)
				{ }
			};

			static size_t _HTTPDownloadWriteData(void *ptr, size_t size, size_t nmemb, void *stream)
			{
				WritingState *state = (WritingState *)stream;
				if (state->writtenBytes == 0)
				{
					state->pResult->responseStartTime = HTTPSystemClock::now();
				}

				size_t written = fwrite(ptr, size, nmemb, state->pStream);
				state->writtenBytes += written;
				return written;
			}
		}

		//------------------------------------------------------------------------------
		// Do HTTP Request and save response to a file in the given path
		HTTPRequestResult DoHTTPDownload(const std::string &downloadFilePath, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
		{
			HTTPRequestResult resultData;

			// Initial Fail Data
			resultData.isRequestSuccess = false;
			resultData.statusCode = 0;

			// Initialize Default curl
			struct curl_slist *headerList = NULL;
			CURL *curl_handle = curl_easy_init();

			curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _Internal::_HTTPDownloadWriteData);

			// Options
			cURL::SetCUrlCommonOptions(curl_handle);
			curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, connectTimeout);

			// Process SSL
			bool isSSL = IsHTTPSecureProtocol(url);

			if (isSSL)
			{
				cURL::SetCUrlCommonSSLOptions(curl_handle);
			}

			// Process Parameters
			std::string encodedParameters;
			if (parameters.size() > 0)
			{
				encodedParameters = EncodePOSTParameters(parameters);

				curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long)encodedParameters.length());
				curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, encodedParameters.c_str());
			}

			// Set Method
			switch (method)
			{
				case RequestMethod::kGet:
					curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
					break;

				case RequestMethod::kPost:
					curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
					curl_easy_setopt(curl_handle, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
					headerList = curl_slist_append(headerList, "Expect:");
					break;
			}

			// Apply Header
			if (headerList != NULL)
			{
				curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerList);
			}

			// Do Request
			FILE *downloadedFile = fopen(downloadFilePath.c_str(), "wb");
			if (downloadedFile)
			{
				_Internal::WritingState state(downloadedFile, &resultData);
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &state);

				resultData.requestStartTime = HTTPSystemClock::now();
				CURLcode res = curl_easy_perform(curl_handle);
				if (res == CURLE_OK)
				{
					resultData.isRequestSuccess = true;

					long http_code = 0;
					curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
					resultData.statusCode = (int)http_code;
				}
				else
				{
					resultData.errorString = curl_easy_strerror(res);
				}
				fclose(downloadedFile);
				resultData.downloadFinishTime = HTTPSystemClock::now();
			}
			else
			{
				resultData.errorString = fmt::format("Cannot write to data file: {0}", downloadFilePath);
			}

			// Clean Up
			curl_easy_cleanup(curl_handle);

			if (headerList != NULL)
			{
				curl_slist_free_all(headerList);
			}

			return resultData;
		}
	}
}
