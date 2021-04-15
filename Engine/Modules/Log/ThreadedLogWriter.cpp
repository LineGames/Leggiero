////////////////////////////////////////////////////////////////////////////////
// ThreadedLogWriter.cpp (Leggiero/Modules - Log)
//
// Threaded Log Writer Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ThreadedLogWriter.h"

// Standard Library
#include <cstring>


namespace Leggiero
{
	namespace Log
	{
		//////////////////////////////////////////////////////////////////////////////// ThreadedLogWriter

		//------------------------------------------------------------------------------
		ThreadedLogWriter::ThreadedLogWriter()
			: m_isRunning(true)
		{
			if (pthread_create(&m_workerThread, NULL, ThreadedLogWriter::_ThreadStartHelper, (void *)this) != 0)
			{
				// Thread Creation Failed
				m_isRunning.store(false);
				return;
			}
		}

		//------------------------------------------------------------------------------
		ThreadedLogWriter::~ThreadedLogWriter()
		{
			bool isRunning = true;
			if (!m_isRunning.compare_exchange_strong(isRunning, false))
			{
				// Thread is Not Running
				return;
			}

			// Publish dummy entry to un-block the thread
			LogEntry dummyEntry(LogLevelType::kNoLog, LogClock::now());
			m_logQueue.enqueue(dummyEntry);

			// wait to thread finish
			pthread_join(m_workerThread, NULL);

			// Discard Allocated Buffers
			std::vector<char> *pBuffer;
			while (m_logMessageStringBufferPool.try_dequeue(pBuffer))
			{
				delete pBuffer;
			}

			while (m_logQueue.try_dequeue(dummyEntry))
			{
				if (dummyEntry.message != nullptr)
				{
					delete dummyEntry.message;
				}
			}
		}

		//------------------------------------------------------------------------------
		void ThreadedLogWriter::WriteLog(LogLevelType level, LogTime time, std::string_view logString)
		{
			if (!m_isRunning.load())
			{
				// Prevent stacking
				return;
			}

			std::vector<char> *pBuffer = _MessageBufferAlloc(logString.length() + 1);
			memcpy(&((*pBuffer)[0]), logString.data(), logString.length());
			(*pBuffer)[logString.length()] = '\0';

			m_logQueue.enqueue(LogEntry(level, time, pBuffer));
		}

		//------------------------------------------------------------------------------
		void *ThreadedLogWriter::_ThreadStartHelper(void *threadThis)
		{
			((ThreadedLogWriter *)threadThis)->_ThreadFunction();
			return nullptr;
		}

		//------------------------------------------------------------------------------
		void ThreadedLogWriter::_ThreadFunction()
		{
			LogEntry readEntry(LogLevelType::kNoLog, LogClock::now());

			while (m_isRunning.load())
			{
				m_logQueue.wait_dequeue(readEntry);
				if (readEntry.message != nullptr)
				{
					_WriteLog(readEntry.level, readEntry.time, std::string_view(&((*readEntry.message)[0]), readEntry.message->size() - 1));
					_MessageBufferFree(readEntry.message);
				}
			}
		}

		//------------------------------------------------------------------------------
		std::vector<char> *ThreadedLogWriter::_MessageBufferAlloc(size_t needSize)
		{
			constexpr size_t kDefaultBufferSize = 128;
			size_t bufferSize = needSize + 4;

			std::vector<char> *pBuffer;
			if (m_logMessageStringBufferPool.try_dequeue(pBuffer))
			{
				pBuffer->reserve(bufferSize);
				pBuffer->resize(needSize);
				return pBuffer;
			}

			pBuffer = new std::vector<char>(
				(kDefaultBufferSize > bufferSize) ? kDefaultBufferSize : bufferSize
				);
			pBuffer->resize(needSize);
			return pBuffer;
		}

		//------------------------------------------------------------------------------
		void ThreadedLogWriter::_MessageBufferFree(std::vector<char> *buffer)
		{
			m_logMessageStringBufferPool.enqueue(buffer);
		}
	}
}
