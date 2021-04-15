////////////////////////////////////////////////////////////////////////////////
// PlatformDefaultLogWriter.h (Leggiero/Modules - Log)
//
// Log Writer writes logs to Platform Default Log Channel
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__PLATFORM_DEFAULT_LOG_WRITER_H
#define __LM_LOG__PLATFORM_DEFAULT_LOG_WRITER_H


// Leggiero.Log
#include "ThreadedLogWriter.h"


namespace Leggiero
{
	namespace Log
	{
		// Platform Default Log Writer
		class PlatformDefaultLogWriter
			: public ThreadedLogWriter
		{
		public:
			PlatformDefaultLogWriter(LogLevelType showLevel = LogLevelType::kWarning, LogLevelType overHideLevel = LogLevelType::kNoLog, bool isPrintTimestamp = true);
			virtual ~PlatformDefaultLogWriter();

		protected:	// ThreadedLogWriter
			// Real Writing Function
			virtual void _WriteLog(LogLevelType level, LogTime time, std::string_view logString) override;

		protected:
			uint8_t	m_showLevel;
			uint8_t	m_overHideLevel;

			bool m_isPrintTimestamp;
		};
	}
}

#endif
