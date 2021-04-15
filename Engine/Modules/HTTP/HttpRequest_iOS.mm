////////////////////////////////////////////////////////////////////////////////
// HttpRequest_iOS.mm (Leggiero/Modules - HTTP)
//
// iOS HTTP Request Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "HttpRequest.h"

// Standard Library
#include <atomic>
#include <cstdlib>

// System Library
#import <Foundation/Foundation.h>

// Leggiero.Utility
#include <Utility/Threading/ThreadSleep.h>


namespace Leggiero
{
	namespace HTTP
	{
		//------------------------------------------------------------------------------
		// Do HTTP Request
		std::shared_ptr<HTTPResponseData> DoHTTPRequest(const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
		{
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
			__block std::shared_ptr<HTTPResponseData> responseData(std::make_shared<HTTPResponseData>());
			__block std::shared_ptr<std::atomic_bool> isCompleted(std::make_shared<std::atomic_bool>(false));
			responseData->requestResult.requestStartTime = HTTPSystemClock::now();
			[[[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
				responseData->requestResult.responseStartTime = HTTPSystemClock::now();
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
				isCompleted->store(true);
			}] resume];
			
			// Wait to Synchronize
			while (!isCompleted->load())
			{
				Utility::Threading::PthreadSleepMS(2);
			}
			responseData->requestResult.downloadFinishTime = HTTPSystemClock::now();
			
			return responseData;
		}
		
		//------------------------------------------------------------------------------
		// Do HTTP Request and save response to a file in the given path
		HTTPRequestResult DoHTTPDownload(const std::string &downloadFilePath, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
		{
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
			id downloadFileUrl = [[NSURL alloc] initFileURLWithPath:[NSString stringWithUTF8String:downloadFilePath.c_str()]];
			__block HTTPRequestResult resultData;
			__block std::shared_ptr<std::atomic_bool> isCompleted(std::make_shared<std::atomic_bool>(false));
			resultData.requestStartTime = HTTPSystemClock::now();
			[[[NSURLSession sharedSession] downloadTaskWithRequest:request completionHandler:^(NSURL *location, NSURLResponse *response, NSError *error) {
				resultData.responseStartTime = HTTPSystemClock::now();
				if (error == nil)
				{
					resultData.isRequestSuccess = true;
					NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
					resultData.statusCode = (int)[httpResponse statusCode];
					
					[[NSFileManager defaultManager] removeItemAtURL:downloadFileUrl error:nil];
					NSError *fileMoveError;
					if ([[NSFileManager defaultManager] moveItemAtURL:location toURL:downloadFileUrl error:&fileMoveError] == NO)
					{
						// Failed to Move File
						resultData.isRequestSuccess = false;
						resultData.statusCode = 0;
						resultData.errorString = [[fileMoveError localizedDescription] UTF8String];
					}
				}
				else
				{
					resultData.isRequestSuccess = false;
					resultData.statusCode = 0;
					resultData.errorString = [[error localizedDescription] UTF8String];
				}
				isCompleted->store(true);
			}] resume];
			
			// Wait to Synchronize
			while (!isCompleted->load())
			{
				Utility::Threading::PthreadSleepMS(2);
			}
			resultData.downloadFinishTime = HTTPSystemClock::now();
			
			return resultData;
		}
	}
}
