////////////////////////////////////////////////////////////////////////////////
// LogTypes.h (Leggiero/Modules - Log)
//
// Common Types Definitions for Log
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__LOG_TYPES_H
#define __LM_LOG__LOG_TYPES_H


// Standard Library
#include <chrono>
#include <cstdint>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Log
	{
		using LogClock = std::chrono::system_clock;
		using LogTime = LogClock::time_point;

		// Log severity level type
		enum class LogLevelType : uint8_t
		{
			kTrace = 0, 
			kDebug = 1,
			kInfo = 2,
			kWarning = 3,
			kError = 4,
			kFatal = 5,
			kNoLog = 6,
		};

		// Convert Log Level to Human Readable Character
		inline constexpr char LogLevelToChar(LogLevelType level)
		{
			switch (level)
			{
				case LogLevelType::kTrace:		return 'T';
				case LogLevelType::kDebug:		return 'D';
				case LogLevelType::kInfo:		return 'I';
				case LogLevelType::kWarning:	return 'W';
				case LogLevelType::kError:		return 'E';
				case LogLevelType::kFatal:		return 'F';
			}
			// Unknown Type
			return 'U';
		}
	}
}

#endif
