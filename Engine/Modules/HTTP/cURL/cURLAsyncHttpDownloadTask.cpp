////////////////////////////////////////////////////////////////////////////////
// cURL/cURLAsyncHttpDownloadTask.cpp (Leggiero/Modules - HTTP)
//
// cURL Async HTTP File Download Implementation
// can be optimized by adopting curl multi interface, ..
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "cURLAsyncHttpDownloadTask.h"

// Standard Library
#include <cstdio>
#include <cstdlib>

#if defined _LEGGIERO_ANDROID
#include <unistd.h>
#endif

// External Library
#include <curl/curl.h>
#include <fmt/format.h>

// Leggiero.Task
#include <Task/TaskManagerComponent.h>

// Leggiero.HTTP
#include "../HttpRequest.h"
#include "cURLUtility.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace cURL
		{
			//////////////////////////////////////////////////////////////////////////////// Internal Helpers

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class WritingState
				{
				public:
					FILE *pStream;
					size_t				writtenBytes;
					HTTPRequestResult *pResult;

				public:
					WritingState(FILE *stream, HTTPRequestResult *result);

				public:
					static size_t _HTTPDownloadWriteData(void *ptr, size_t size, size_t nmemb, void *stream);
					static int _HTTPDownloadXferInfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
				};

				//------------------------------------------------------------------------------
				WritingState::WritingState(FILE *stream, HTTPRequestResult *result)
					: writtenBytes(0)
					, pStream(stream), pResult(result)
				{
				}

				//------------------------------------------------------------------------------
				size_t WritingState::_HTTPDownloadWriteData(void *ptr, size_t size, size_t nmemb, void *stream)
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

				//------------------------------------------------------------------------------
				int WritingState::_HTTPDownloadXferInfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
				{
					CURLHTTPDownloadTask *pTask = (CURLHTTPDownloadTask *)p;

					pTask->m_estimatedDownloadSize = (size_t)dltotal;
					pTask->m_downloadedSize = (size_t)dlnow;

					if (pTask->m_isCancelRequested.load())
					{
						pTask->m_isCanceled.store(true);
						return 1;
					}

					return 0;
				}
			}


			//////////////////////////////////////////////////////////////////////////////// CURLHTTPDownloadTask

			//------------------------------------------------------------------------------
			CURLHTTPDownloadTask::CURLHTTPDownloadTask(const std::string &downloadFilePath, bool isBackgroundTask, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, int connectTimeout)
				: HTTPDownloadTask(isBackgroundTask ? Task::TaskPriorityClass::kBackground : Task::TaskPriorityClass::kDefault)
				, m_downloadFilePath(downloadFilePath), m_requestURL(url), m_requestMethod(method), m_requestParameters(parameters), m_optionConnectTimeout(connectTimeout)
				, m_estimatedDownloadSize(0), m_downloadedSize(0)
				, m_isCancelRequested(false), m_isCanceled(false)
			{
				m_result.isRequestSuccess = false;
				m_result.statusCode = 0;
			}

			//------------------------------------------------------------------------------
			CURLHTTPDownloadTask::~CURLHTTPDownloadTask()
			{
			}

			//------------------------------------------------------------------------------
			Task::TaskDoneResult CURLHTTPDownloadTask::Do()
			{
				// Initialize Default curl
				struct curl_slist *headerList = NULL;
				CURL *curl_handle = curl_easy_init();

				curl_easy_setopt(curl_handle, CURLOPT_URL, m_requestURL.c_str());

				curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _Internal::WritingState::_HTTPDownloadWriteData);

				// Options
				SetCUrlCommonOptions(curl_handle);
				curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, m_optionConnectTimeout);

				// Progress
				curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
				curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, _Internal::WritingState::_HTTPDownloadXferInfo);
				curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, this);

				// Process SSL
				bool isSSL = IsHTTPSecureProtocol(m_requestURL);

				if (isSSL)
				{
					SetCUrlCommonSSLOptions(curl_handle);
				}

				// Process Parameters
				std::string encodedParameters;
				if (m_requestParameters.size() > 0)
				{
					encodedParameters = EncodePOSTParameters(m_requestParameters);

					curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long)encodedParameters.length());
					curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, encodedParameters.c_str());
				}

				// Set Method
				switch (m_requestMethod)
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
				FILE *downloadedFile = fopen(m_downloadFilePath.c_str(), "wb");
				if (downloadedFile)
				{
					_Internal::WritingState state(downloadedFile, &(this->m_result));
					curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &state);

					m_result.requestStartTime = HTTPSystemClock::now();
					CURLcode res = curl_easy_perform(curl_handle);
					if (res == CURLE_OK)
					{
						m_result.isRequestSuccess = true;

						long http_code = 0;
						curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
						m_result.statusCode = (int)http_code;

						fseek(downloadedFile, 0L, SEEK_END);
						m_downloadedSize = (size_t)ftell(downloadedFile);
					}
					else
					{
						m_result.errorString = curl_easy_strerror(res);
					}

					#if defined _LEGGIERO_ANDROID
						fflush(downloadedFile);
						fsync(fileno(downloadedFile));
					#endif
					fclose(downloadedFile);
					m_result.responseStartTime = HTTPSystemClock::now();
				}
				else
				{
					m_result.errorString = fmt::format("Cannot write to data file: {0}", m_downloadFilePath);
				}

				// Clean Up
				curl_easy_cleanup(curl_handle);

				if (headerList != NULL)
				{
					curl_slist_free_all(headerList);
				}

				return Task::TaskDoneResult(Task::TaskDoneResult::ResultType::kFinished);
			}

			//------------------------------------------------------------------------------
			float CURLHTTPDownloadTask::GetEstimatedProgress(bool isClipped)
			{
				if (IsFinished())
				{
					return 1.0f;
				}
				if (m_estimatedDownloadSize == 0)
				{
					return 0.0f;
				}
				float progress = (float)m_downloadedSize / (float)m_estimatedDownloadSize;
				if (isClipped && progress > 1.0f)
				{
					progress = 1.0f;
				}
				return progress;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<CURLHTTPDownloadTask> CURLHTTPDownloadTask::StartHTTPDownloadAsync(Task::TaskManagerComponent *taskManager, const std::string &downloadFilePath, bool isBackgroundTask, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
			{
				std::shared_ptr<CURLHTTPDownloadTask> downloadTask(std::make_shared<CURLHTTPDownloadTask>(downloadFilePath, isBackgroundTask, url, method, parameters, connectTimeout));

				if (!taskManager->ExecuteTask(downloadTask))
				{
					// Something Wrong
					return nullptr;
				}

				return downloadTask;
			}
		}
	}
}
