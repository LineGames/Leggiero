////////////////////////////////////////////////////////////////////////////////
// PlatformDefaultLogWriter_WinPC.cpp (Leggiero/Modules - Log)
//
// WinPC Platform Default Log Writer Implementation
// Logs will be printed as Windows' debug string, which can read via DebugView
// or IDE's debug window.
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "PlatformDefaultLogWriter.h"

// Leggiero.Platform.WinPC
#include <WindowsUtility.h>


namespace Leggiero
{
	namespace Log
	{
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

				Platform::Windows::Utility::PrintDebugStringFormat("[%c] (%lld.%06d) \t%s\n", LogLevelToChar(level), timestampPart, subtimestampPart, logString.data());
			}
			else
			{
				Platform::Windows::Utility::PrintDebugStringFormat("[%c] %s\n", LogLevelToChar(level), logString.data());
			}
		}
	}
}
