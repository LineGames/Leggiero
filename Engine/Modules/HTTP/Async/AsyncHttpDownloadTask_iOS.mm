////////////////////////////////////////////////////////////////////////////////
// Async/AsyncHttpDownloadTask_iOS.mm (Leggiero/Modules - HTTP)
//
// iOS Async HTTP Download Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AsyncHttpDownloadTask.h"

// Standard Library
#include <atomic>
#include <cstdlib>

// System Library
#import <Foundation/Foundation.h>

// Leggiero.HTTP
#include "../HttpUtility.h"
#include "AsyncHttp_iOS.h"


// Forward Declaration
namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			namespace iOS
			{
				class NSURLSessionHTTPDownloadTask;
			}
		}
	}
}


#pragma mark - Objective-C Interface
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Delegate to process download
@interface HTTPDownloadDelegate : NSObject<NSURLSessionDownloadDelegate>
{
@public
    std::weak_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask> asyncTask;
    id downloadFileUrl;
}

- (void)setAsyncTask:(std::shared_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask>)sharedAsyncTask;

- (void)setDownloadFileUrl:(id)fileUrl;

@end


#pragma mark - Download Task and C++ Function Implementation
////////////////////////////////////////////////////////////////////////////////

namespace Leggiero
{
	namespace HTTP
	{
		namespace Async
		{
			namespace iOS
			{
				//////////////////////////////////////////////////////////////////////////////// NSURLSessionHTTPDownloadTask
				
				//------------------------------------------------------------------------------
				// NSURLSession based asynchronous HTTP download Task
				class NSURLSessionHTTPDownloadTask
					: public HTTPDownloadTask
				{
					friend std::shared_ptr<HTTPDownloadTask> StartHTTPDownloadAsync(const std::string &, bool, const std::string &, RequestMethod, const POSTParameterMap &, long);
					
				public:
					NSURLSessionHTTPDownloadTask(Task::TaskPriorityClass priority);
					virtual ~NSURLSessionHTTPDownloadTask() { }

				public:	// ITask
					// Do Real Task Works
					virtual Task::TaskDoneResult Do() override
					{
						// Nothing to do in this function
						return Task::TaskDoneResult(Task::TaskDoneResult::ResultType::kFinished);
					}

				public:	// IAsyncValueTask
					// Check whether the task have result value
					virtual bool HasValue() const override
					{
						Task::TaskState taskState = this->GetTaskState();
						return (Utility::SyntacticSugar::HasFlag(taskState, Task::TaskState::kJobFinished)
							&& !Utility::SyntacticSugar::HasFlag(taskState, Task::TaskState::kHasError));
					}

					// Get result value
					virtual HTTPRequestResult GetValue() override { return m_result; }

				public:	// Async::HTTPDownloadTask
					// Get estimated file size. 0 if unavailable.
					virtual size_t GetEstimatedFileSize() override { return m_estimatedDownloadSize; }

					// Get currently downloaded data size
					virtual size_t GetDownloadedSize() override { return m_downloadedSize; }

					// Get progress value. If total estimated size is unavailable, always return 0.0.
					// Result value may in range [0.0, 1.0], but can exceed the range because of invalid estimation of size. isClipped can prevent it.
					virtual float GetEstimatedProgress(bool isClipped = true) override;
					
					// Request download stop
					virtual void RequestToCancelDownload() override;

					// Check whether if the downloading finished by force before end of download.
					virtual bool IsCanceledDownload() override
					{
						return m_isCanceled.load();
					}
					
				public:
					void NotifyDownloadProgress(size_t estimatedDownloadSize, size_t downloadedSize);
					void NotifyTaskFinished();

				protected:
					size_t m_estimatedDownloadSize;
					size_t m_downloadedSize;

				public:
					id m_downloadTask;
					
					std::atomic_bool	m_isCancelRequested;

					HTTPRequestResult	m_result;
					std::atomic_bool	m_isCanceled;
				};
				
				
				//------------------------------------------------------------------------------
				NSURLSessionHTTPDownloadTask::NSURLSessionHTTPDownloadTask(Task::TaskPriorityClass priority)
					: HTTPDownloadTask(priority)
					, m_downloadTask(nil)
					, m_estimatedDownloadSize(0), m_downloadedSize(0)
					, m_isCancelRequested(false), m_isCanceled(false)
				{
					m_result.isRequestSuccess = false;
					m_result.statusCode = 0;
				}
				
				//------------------------------------------------------------------------------
				float NSURLSessionHTTPDownloadTask::GetEstimatedProgress(bool isClipped)
				{
					if (this->IsFinished())
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
				void NSURLSessionHTTPDownloadTask::RequestToCancelDownload()
				{
					if ([m_downloadTask state] == NSURLSessionTaskStateCompleted)
					{
						// Already Finished
						return;
					}
					
					bool notCanceledByAnother = false;
					if (!m_isCancelRequested.compare_exchange_strong(notCanceledByAnother, true))
					{
						// Duplicated Cancel
						return;
					}
					
					[m_downloadTask cancel];
				}
				
				//------------------------------------------------------------------------------
				void NSURLSessionHTTPDownloadTask::NotifyDownloadProgress(size_t estimatedDownloadSize, size_t downloadedSize)
				{
					m_estimatedDownloadSize = estimatedDownloadSize;
					m_downloadedSize = downloadedSize;
				}
				
				//------------------------------------------------------------------------------
				void NSURLSessionHTTPDownloadTask::NotifyTaskFinished()
				{
					m_result.downloadFinishTime = HTTPSystemClock::now();
                    m_currentState.store(Task::TaskState::kDone);
				}
				
				
				//////////////////////////////////////////////////////////////////////////////// Function Interface
            
				//------------------------------------------------------------------------------
				std::shared_ptr<HTTPDownloadTask> StartHTTPDownloadAsync(const std::string &downloadFilePath, bool isBackgroundTask, const std::string &url, RequestMethod method, const POSTParameterMap &parameters, long connectTimeout)
				{
					Task::TaskPriorityClass taskPriority = Task::TaskPriorityClass::kDefault;
					if (isBackgroundTask)
					{
						taskPriority = Task::TaskPriorityClass::kBackground;
					}
					
					__block std::shared_ptr<NSURLSessionHTTPDownloadTask> downloadTask(std::make_shared<NSURLSessionHTTPDownloadTask>(taskPriority));
					downloadTask->State().store(Task::TaskState::kProcessing);
					
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
					
					// Make a Session
					NSURLSessionConfiguration *sessionConfiguration = [NSURLSessionConfiguration defaultSessionConfiguration];
					
					id delegate = [[HTTPDownloadDelegate alloc] init];
					[delegate setAsyncTask:downloadTask];
					[delegate setDownloadFileUrl:[[NSURL alloc] initFileURLWithPath:[NSString stringWithUTF8String:downloadFilePath.c_str()]]];
					
					NSOperationQueue *operationQueue = [NSOperationQueue mainQueue];
					
					id customizedSession = [NSURLSession sessionWithConfiguration:sessionConfiguration delegate:delegate delegateQueue:operationQueue];
					
					// Start Download
					downloadTask->m_downloadTask = [customizedSession downloadTaskWithRequest:request];
					if (isBackgroundTask)
					{
						[downloadTask->m_downloadTask setPriority:NSURLSessionTaskPriorityLow];
					}
					
					downloadTask->m_result.requestStartTime = HTTPSystemClock::now();
					[downloadTask->m_downloadTask resume];
					downloadTask->m_result.responseStartTime = HTTPSystemClock::now();
					
					return downloadTask;
				}
			}
		}
	}
}


#pragma mark - Objective-C Implementation
////////////////////////////////////////////////////////////////////////////////

@implementation HTTPDownloadDelegate

//------------------------------------------------------------------------------
- (id)init
{
    self = [super init];
    
    return self;
}

//------------------------------------------------------------------------------
- (void)setAsyncTask:(std::shared_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask>)sharedAsyncTask
{
    asyncTask = sharedAsyncTask;
}

//------------------------------------------------------------------------------
- (void)setDownloadFileUrl:(id)fileUrl
{
    downloadFileUrl = fileUrl;
}

//------------------------------------------------------------------------------
- (void)URLSession:(NSURLSession *)session
      downloadTask:(NSURLSessionDownloadTask *)downloadTask
didFinishDownloadingToURL:(NSURL *)location
{
    [[NSFileManager defaultManager] removeItemAtURL:downloadFileUrl error:nil];
    NSError *fileMoveError;
    [[NSFileManager defaultManager] moveItemAtURL:location toURL:downloadFileUrl error:&fileMoveError];
    
    std::shared_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask> downloadAsyncTask(asyncTask.lock());
    if (!downloadAsyncTask)
    {
        return;
    }
    
    if (fileMoveError == nil)
    {
        downloadAsyncTask->m_result.isRequestSuccess = true;
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)[downloadAsyncTask->m_downloadTask response];
        downloadAsyncTask->m_result.statusCode = (int)[httpResponse statusCode];
    }
    else
    {
        // Failed to Move File
        downloadAsyncTask->m_result.isRequestSuccess = false;
        downloadAsyncTask->m_result.statusCode = 0;
        downloadAsyncTask->m_result.errorString = [[fileMoveError localizedDescription] UTF8String];
    }
    
    downloadAsyncTask->m_downloadTask = nil;
    downloadAsyncTask->NotifyTaskFinished();
}

//------------------------------------------------------------------------------
- (void)URLSession:(NSURLSession *)session
      downloadTask:(NSURLSessionDownloadTask *)downloadTask
      didWriteData:(int64_t)bytesWritten
 totalBytesWritten:(int64_t)totalBytesWritten
totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite
{
    std::shared_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask> downloadAsyncTask(asyncTask.lock());
    if (!downloadAsyncTask)
    {
        return;
    }
    
    downloadAsyncTask->NotifyDownloadProgress((size_t)totalBytesExpectedToWrite, (size_t)totalBytesWritten);
}

//------------------------------------------------------------------------------
- (void)URLSession:(NSURLSession *)session
      downloadTask:(NSURLSessionDownloadTask *)downloadTask
 didResumeAtOffset:(int64_t)fileOffset
expectedTotalBytes:(int64_t)expectedTotalBytes
{
    std::shared_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask> downloadAsyncTask(asyncTask.lock());
    if (!downloadAsyncTask)
    {
        return;
    }
    
    downloadAsyncTask->NotifyDownloadProgress((size_t)expectedTotalBytes, (size_t)fileOffset);
}

//------------------------------------------------------------------------------
- (void)URLSession:(NSURLSession *)session
              task:(NSURLSessionTask *)task
didCompleteWithError:(NSError *)error
{
    std::shared_ptr<Leggiero::HTTP::Async::iOS::NSURLSessionHTTPDownloadTask> downloadAsyncTask(asyncTask.lock());
    if (!downloadAsyncTask)
    {
        return;
    }
    
    downloadAsyncTask->m_result.isRequestSuccess = false;
    downloadAsyncTask->m_result.statusCode = 0;
    if (error != nil)
    {
        downloadAsyncTask->m_result.errorString = [[error localizedDescription] UTF8String];
    }
    
    if (downloadAsyncTask->m_isCancelRequested.load())
    {
        downloadAsyncTask->m_isCanceled.store(true);
    }
    
    downloadAsyncTask->m_downloadTask = nil;
    downloadAsyncTask->NotifyTaskFinished();
}

@end
