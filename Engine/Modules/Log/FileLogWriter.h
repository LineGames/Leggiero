////////////////////////////////////////////////////////////////////////////////
// FileLogWriter.h (Leggiero/Modules - Log)
//
// Log Writer to File
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__FILE_LOG_WRITER_H
#define __LM_LOG__FILE_LOG_WRITER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>
#include <cstdio>

// Leggiero.Log
#include "ThreadedLogWriter.h"


namespace Leggiero
{
	namespace Log
	{
		// File Log Writer
		class FileLogWriter
			: public ThreadedLogWriter
		{
		public:
			FileLogWriter(const char *filePath, LogLevelType showLevel = LogLevelType::kWarning, LogLevelType overHideLevel = LogLevelType::kNoLog);
			FileLogWriter(FILE *targetFile, LogLevelType showLevel = LogLevelType::kWarning, LogLevelType overHideLevel = LogLevelType::kNoLog);
			virtual ~FileLogWriter();

		protected:	// ThreadedLogWriter
			// Real Writing Function
			virtual void _WriteLog(LogLevelType level, LogTime time, std::string_view logString) override;

		protected:
			bool	m_isOpenFile;
			FILE	*m_targetFile;

			uint8_t	m_showLevel;
			uint8_t	m_overHideLevel;
		};
	}
}

#endif
