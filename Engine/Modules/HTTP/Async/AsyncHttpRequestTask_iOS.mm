////////////////////////////////////////////////////////////////////////////////
// Async/AsyncHttpRequestTask_iOS.mm (Leggiero/Modules - HTTP)
//
// iOS Async HTTP Request Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsyncHttpRequestTask.h"

// Standard Library
#include <atomic>
#include <cstdlib>

// System Library
#import <Foundation/Foundation.h>

// Leggiero.HTTP
#include "../HttpUtility.h"
#include "AsyncHttp_iOS.h"


namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			namespace iOS
			{
				//////////////////////////////////////////////////////////////////////////////// NSURLSessionHTTPRequestTask
				
				//------------------------------------------------------------------------------
				// NSURLSession based asynchronous HTTP Request Task
				class NSURLSessionHTTPRequestTask
					: public HTTPRequestTask
				{
					friend std::shared_ptr<HTTPRequestTask> StartHTTPRequestAsync(const std::string &, RequestMethod, const POSTParameterMap &, long);
					
				public:
					NSURLSessionHTTPRequestTask()
					{
					}

					virtual ~NSURLSessionHTTPRequestTask() { }

				public:	// ITask
					// Do Real Task Works
					virtual Task::TaskDoneResult Do() override
					{
						// Nothing to do in this function
						return Task::TaskDoneResult(Task::TaskDoneResult::ResultType::kFinished);
					}
					
				public:	// IAsyncValueTask
					// Check whether the task have result value
					virtual bool HasValue() const override { return (bool)m_result; }

					// Get result value
					virtual std::shared_ptr<HTTPResponseData> GetValue() override { return m_result; }

				protected:
					std::shared_ptr<HTTPResponseData> m_result;
				};
				
				
				//////////////////////////////////////////////////////////////////////////////// AsyncHTTPResponseData
				
				//------------------------------------------------------------------------------
				// Dummy sub-class to access protected members
				class AsyncHTTPResponseData
					: public HTTPResponseData
				{
					friend std::shared_ptr<HTTPRequestTask> StartHTTPRequestAsync(const std::string &, RequestMethod, const POSTParameterMap &, long);
					
				public:
					virtual ~AsyncHTTPResponseData() { }
				};
				
				
				//////////////////////////////////////////////////////////////////////////////// Function Interface
            
				//------------------------------------------------------------------------------
				std::shared_ptr<HTTPRequestTask> StartHTTPRequestAsync(const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
				{
					__block std::shared_ptr<NSURLSessionHTTPRequestTask> requestTask(std::make_shared<NSURLSessionHTTPRequestTask>());
                    requestTask->State().store(Task::TaskState::kProcessing);
					
					id request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
				
					// Not sure to work...
					[request setTimeoutInterval:connectTimeout];
					
					// Process Parameters
					if (parameters.size() > 0)
					{
						std::string encodedParameters(EncodePOSTParameters(parameters));
						id parameterData = [NSData dataWithBytes:encodedParameters.c_str() length:encodedParameters.length()];
						[request setHTTPBody:parameterData];
					}

					// Set Method
					switch (method)
					{
						case RequestMethod::kGet:
							[request setHTTPMethod:@"GET"];
							break;

						case RequestMethod::kPost:
							[request setHTTPMethod:@"POST"];
							break;
					}
					
					// Do Request
					HTTPSystemClock::time_point beforeTime(HTTPSystemClock::now());
					[[[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
						HTTPSystemClock::time_point responseTime(HTTPSystemClock::now());
						std::shared_ptr<AsyncHTTPResponseData> responseData(std::make_shared<AsyncHTTPResponseData>());
						responseData->requestResult.requestStartTime = beforeTime;
						responseData->requestResult.responseStartTime = responseTime;
						if (error == nil)
						{
							responseData->requestResult.isRequestSuccess = true;
							NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
							responseData->requestResult.statusCode = (int)[httpResponse statusCode];
							
							responseData->m_currentDataSize = [data length];
							responseData->m_currentBufferSize = responseData->m_currentDataSize;
							responseData->m_dataBuffer = malloc(responseData->m_currentDataSize);
							memcpy(responseData->m_dataBuffer, [data bytes], responseData->m_currentDataSize);
						}
						else
						{
							responseData->requestResult.isRequestSuccess = false;
							responseData->requestResult.statusCode = 0;
							responseData->requestResult.errorString = [[error localizedDescription] UTF8String];
						}
						
						responseData->requestResult.downloadFinishTime = HTTPSystemClock::now();
						requestTask->m_result = responseData;
                        requestTask->State().store(Task::TaskState::kDone);
					}] resume];
					
					return requestTask;
				}
			}
		}
	}
}
