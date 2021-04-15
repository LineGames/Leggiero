////////////////////////////////////////////////////////////////////////////////
// ThreadedLogWriter.h (Leggiero/Modules - Log)
//
// Threaded Log Writer Base Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__THREADED_LOG_WRITER_H
#define __LM_LOG__THREADED_LOG_WRITER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <vector>

// External Library
#include <pthread.h>
#include <concurrentqueue/concurrentqueue.h>
#include <readerwriterqueue/readerwriterqueue.h>

// Leggiero.Log
#include "ILogWriter.h"


namespace Leggiero
{
	namespace Log
	{
		// Threaded Log Writer
		class ThreadedLogWriter
			: public ILogWriter
		{
		public:
			ThreadedLogWriter();
			virtual ~ThreadedLogWriter();

		public:	// ILogWriter
			virtual void WriteLog(LogLevelType level, LogTime time, std::string_view logString) override;

		protected:
			// Real Writing Function
			virtual void _WriteLog(LogLevelType level, LogTime time, std::string_view logString) = 0;

		private:
			pthread_t m_workerThread;

			static void *_ThreadStartHelper(void *threadThis);
			virtual void _ThreadFunction();

		protected:
			std::vector<char> *_MessageBufferAlloc(size_t needSize);
			void _MessageBufferFree(std::vector<char> *buffer);

		protected:
			std::atomic_bool m_isRunning;

			moodycamel::ConcurrentQueue<std::vector<char> *> m_logMessageStringBufferPool;

			struct LogEntry
			{
				LogLevelType		level;
				LogTime				time;
				std::vector<char>	*message;

				LogEntry(LogLevelType logLevel, LogTime at, std::vector<char> *logMessage = nullptr)
					: level(logLevel), time(at), message(logMessage)
				{ }
			};

			moodycamel::BlockingReaderWriterQueue<LogEntry> m_logQueue;
		};
	}
}

#endif
