////////////////////////////////////////////////////////////////////////////////
// Logger.h (Leggiero/Modules - Log)
//
// Leggiero Logger
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__LOGGER_H
#define __LM_LOG__LOGGER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdarg>
#include <memory>
#include <string_view>
#include <vector>

// External Library
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Log
#include "LogTypes.h"


namespace Leggiero
{
	namespace Log
	{
		// Forward Declarations
		class ILogWriter;


		// Logger
		class Logger
			: private Utility::SyntacticSugar::NonCopyable
		{
		public:
			Logger();
			~Logger();

		public:
			void Log(LogLevelType logType, LogTime time, std::string_view log);
			void Log(LogLevelType logType, std::string_view log);

			void LogPrintf(LogLevelType logType, LogTime time, const char *const format, ...);
			void LogPrintf(LogLevelType logType, const char *const format, ...);

		public:
			void RegisterLogWriter(std::shared_ptr<ILogWriter> writer);
			void UnRegisterLogWriter(std::shared_ptr<ILogWriter> writer);

			std::vector<std::shared_ptr<ILogWriter> > GetAllLogWriters();

		private:
			LogTime _CurrentTime();

			std::vector<char> *_MessageBufferAlloc(size_t needSize);
			void _MessageBufferFree(std::vector<char> *buffer);

			void _LogPrintf(LogLevelType logType, LogTime time, const char *const format, va_list args);

		private:
			Utility::Threading::SafePthreadRWLock		m_writerListLock;
			std::vector<std::shared_ptr<ILogWriter> >	m_writers;

			moodycamel::ConcurrentQueue<std::vector<char> *> m_logMessageBuffer;
		};


		// Get Engine-wide Main Logger
		Logger &MainLogger();
	}
}

#endif
