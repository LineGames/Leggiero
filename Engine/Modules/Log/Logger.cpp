////////////////////////////////////////////////////////////////////////////////
// Logger.cpp (Leggiero/Modules - Log)
//
// Leggiero Logger Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Logger.h"

// Standard Library
#include <cstdio>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ThreadSleep.h>

// Leggiero.Log
#include "ILogWriter.h"


namespace Leggiero
{
	namespace Log
	{
		//////////////////////////////////////////////////////////////////////////////// Logger

		//------------------------------------------------------------------------------
		Logger::Logger()
		{
		}

		//------------------------------------------------------------------------------
		Logger::~Logger()
		{
			std::vector<char> *pBuffer;
			while (m_logMessageBuffer.try_dequeue(pBuffer))
			{
				delete pBuffer;
			}
		}

		//------------------------------------------------------------------------------
		void Logger::Log(LogLevelType logType, LogTime time, std::string_view log)
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_writerListLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_writerListLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				for (std::shared_ptr<ILogWriter> &currentWriter : m_writers)
				{
					currentWriter->WriteLog(logType, time, log);
				}
			}
		}

		//------------------------------------------------------------------------------
		void Logger::Log(LogLevelType logType, std::string_view log)
		{
			Log(logType, _CurrentTime(), log);
		}

		//------------------------------------------------------------------------------
		void Logger::LogPrintf(LogLevelType logType, LogTime time, const char *const format, ...)
		{
			va_list arg_list;
			va_start(arg_list, format);
			_LogPrintf(logType, time, format, arg_list);
			va_end(arg_list);
		}

		//------------------------------------------------------------------------------
		void Logger::LogPrintf(LogLevelType logType, const char *const format, ...)
		{
			va_list arg_list;
			va_start(arg_list, format);
			_LogPrintf(logType, _CurrentTime(), format, arg_list);
			va_end(arg_list);
		}

		//------------------------------------------------------------------------------
		void Logger::RegisterLogWriter(std::shared_ptr<ILogWriter> writer)
		{
			int writeLockResult = pthread_rwlock_wrlock(&m_writerListLock.GetLock());
			if (writeLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_writerListLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

				m_writers.push_back(writer);
			}
			else
			{
				// Something Wrong
			}
		}

		//------------------------------------------------------------------------------
		void Logger::UnRegisterLogWriter(std::shared_ptr<ILogWriter> writer)
		{
			int writeLockResult = pthread_rwlock_wrlock(&m_writerListLock.GetLock());
			if (writeLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_writerListLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_WRITE_LOCK, releaseLockFunc);

				for (std::vector<std::shared_ptr<ILogWriter> >::iterator it = m_writers.begin(); it != m_writers.end(); ++it)
				{
					if (*it == writer)
					{
						m_writers.erase(it);
						break;
					}
				}
			}
			else
			{
				// Something Wrong, but anyway go
				for (std::vector<std::shared_ptr<ILogWriter> >::iterator it = m_writers.begin(); it != m_writers.end(); ++it)
				{
					if (*it == writer)
					{
						m_writers.erase(it);
						break;
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		std::vector<std::shared_ptr<ILogWriter> > Logger::GetAllLogWriters()
		{
			int readLockResult;
			while ((readLockResult = pthread_rwlock_rdlock(&m_writerListLock.GetLock())) == EAGAIN)
			{
				sched_yield();
			}
			if (readLockResult == 0)
			{
				pthread_rwlock_t *listLock = &m_writerListLock.GetLock();
				auto releaseLockFunc = [listLock]() mutable { pthread_rwlock_unlock(listLock); };
				FINALLY_OF_BLOCK(_RELEASE_READ_LOCK, releaseLockFunc);

				std::vector<std::shared_ptr<ILogWriter> > copied(m_writers);
				return copied;
			}
			return std::vector<std::shared_ptr<ILogWriter> >();
		}

		//------------------------------------------------------------------------------
		LogTime Logger::_CurrentTime()
		{
			return LogClock::now();
		}

		//------------------------------------------------------------------------------
		std::vector<char> *Logger::_MessageBufferAlloc(size_t needSize)
		{
			constexpr size_t kDefaultBufferSize = 128;
			size_t bufferSize = needSize + 4;

			std::vector<char> *pBuffer;
			if (m_logMessageBuffer.try_dequeue(pBuffer))
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
		void Logger::_MessageBufferFree(std::vector<char> *buffer)
		{
			m_logMessageBuffer.enqueue(buffer);
		}

		//------------------------------------------------------------------------------
		void Logger::_LogPrintf(LogLevelType logType, LogTime time, const char *const format, va_list args)
		{
			int logStringLength = vsnprintf(nullptr, 0, format, args);
			if (logStringLength >= 0)
			{
				size_t bufferSize = (size_t)logStringLength + 1;
				std::vector<char> *pBuffer = _MessageBufferAlloc(bufferSize);
				int writtenLength = vsnprintf(&((*pBuffer)[0]), bufferSize, format, args);
				if (writtenLength > 0)
				{
					Log(logType, time, std::string_view(&((*pBuffer)[0]), writtenLength));
				}
				_MessageBufferFree(pBuffer);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// Main Logger

		namespace _Internal
		{
			Logger g_mainLogger;
		}

		//------------------------------------------------------------------------------
		// Get Engine-wide Main Logger
		Logger &MainLogger()
		{
			return _Internal::g_mainLogger;
		}
	}
}
