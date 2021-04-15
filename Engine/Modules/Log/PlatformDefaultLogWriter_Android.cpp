////////////////////////////////////////////////////////////////////////////////
// PlatformDefaultLogWriter_Android.cpp (Leggiero/Modules - Log)
//
// Android Platform Default Log Writer Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "PlatformDefaultLogWriter.h"

// Android
#include <android/log.h>


namespace Leggiero
{
	namespace Log
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Utility

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			android_LogPriority ConvertLogPriority(LogLevelType logLevel)
			{
				switch (logLevel)
				{
					case LogLevelType::kTrace: return ANDROID_LOG_VERBOSE;
					case LogLevelType::kDebug: return ANDROID_LOG_DEBUG;
					case LogLevelType::kInfo: return ANDROID_LOG_INFO;
					case LogLevelType::kWarning: return ANDROID_LOG_WARN;
					case LogLevelType::kError: return ANDROID_LOG_ERROR;
					case LogLevelType::kFatal: return ANDROID_LOG_FATAL;
				}
				return ANDROID_LOG_UNKNOWN;
			}
		}


		//////////////////////////////////////////////////////////////////////////////// PlatformDefaultLogWriter

		//------------------------------------------------------------------------------
		PlatformDefaultLogWriter::PlatformDefaultLogWriter(LogLevelType showLevel, LogLevelType overHideLevel, bool isPrintTimestamp)
			: m_showLevel(static_cast<uint8_t>(showLevel)), m_overHideLevel(static_cast<uint8_t>(overHideLevel)), m_isPrintTimestamp(isPrintTimestamp)
		{
		}

		//------------------------------------------------------------------------------
		PlatformDefaultLogWriter::~PlatformDefaultLogWriter()
		{
		}

		//------------------------------------------------------------------------------
		// Real Writing Function
		void PlatformDefaultLogWriter::_WriteLog(LogLevelType level, LogTime time, std::string_view logString)
		{
			uint8_t levelNumber = static_cast<uint8_t>(level);
			if (levelNumber < m_showLevel || levelNumber >= m_overHideLevel)
			{
				return;
			}

			if (m_isPrintTimestamp)
			{
				int64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
				int64_t timestampPart = timestamp / 1000000;
				int subtimestampPart = (int)(timestamp % 1000000);

				__android_log_print(_Internal::ConvertLogPriority(level), "LEGGIERO", "(%lld.%06d) \t%s", static_cast<long long>(timestampPart), subtimestampPart, logString.data());
			}
			else
			{
				__android_log_write(_Internal::ConvertLogPriority(level), "LEGGIERO", logString.data());
			}
		}
	}
}
