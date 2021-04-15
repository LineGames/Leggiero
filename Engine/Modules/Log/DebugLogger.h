////////////////////////////////////////////////////////////////////////////////
// DebugLogger.h (Leggiero/Modules - Log)
//
// Logger for Debug which doesn't make overhead on Release Build
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__DEBUG_LOGGER_H
#define __LM_LOG__DEBUG_LOGGER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Log
#include "Logger.h"


namespace Leggiero
{
	namespace Log
	{

#ifdef _LEGGIERO_DEBUG

		using DebugLogger = Logger;

		DebugLogger &GetDebugLogger();

#else	// NOT _LEGGIERO_DEBUG

		// Dummy Debug Logger
		class DebugLogger
		{
		public:	// Dummy Inline Functions
			void Log(LogLevelType logType, LogTime time, std::string_view log) { }
			void Log(LogLevelType logType, std::string_view log) { }

			void LogPrintf(LogLevelType logType, LogTime time, const char *const format, ...) { }
			void LogPrintf(LogLevelType logType, const char *const format, ...) { }

			void RegisterLogWriter(std::shared_ptr<ILogWriter> writer) { }
			void UnRegisterLogWriter(std::shared_ptr<ILogWriter> writer) { }
		};

		namespace _Internal
		{
			extern DebugLogger g_debugLogger;
		}

		inline DebugLogger &GetDebugLogger() { return _Internal::g_debugLogger; }

#endif	// _LEGGIERO_DEBUG

	}
}

#endif
