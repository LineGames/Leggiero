////////////////////////////////////////////////////////////////////////////////
// FileLogWriter.cpp (Leggiero/Modules - Log)
//
// File Log Writer Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FileLogWriter.h"


namespace Leggiero
{
	namespace Log
	{
		//////////////////////////////////////////////////////////////////////////////// FileLogWriter

		//------------------------------------------------------------------------------
		FileLogWriter::FileLogWriter(const char *filePath, LogLevelType showLevel, LogLevelType overHideLevel)
			: m_isOpenFile(true), m_showLevel(static_cast<uint8_t>(showLevel)), m_overHideLevel(static_cast<uint8_t>(overHideLevel))
		{
			m_targetFile = fopen(filePath, "w+t");
		}

		//------------------------------------------------------------------------------
		FileLogWriter::FileLogWriter(FILE *targetFile, LogLevelType showLevel, LogLevelType overHideLevel)
			: m_isOpenFile(false), m_targetFile(targetFile), m_showLevel(static_cast<uint8_t>(showLevel)), m_overHideLevel(static_cast<uint8_t>(overHideLevel))
		{
		}

		//------------------------------------------------------------------------------
		FileLogWriter::~FileLogWriter()
		{
			if (m_isOpenFile)
			{
				fflush(m_targetFile);
				fclose(m_targetFile);
			}
		}

		//------------------------------------------------------------------------------
		// Real Writing Function
		void FileLogWriter::_WriteLog(LogLevelType level, LogTime time, std::string_view logString)
		{
			uint8_t levelNumber = static_cast<uint8_t>(level);
			if (levelNumber < m_showLevel || levelNumber >= m_overHideLevel)
			{
				return;
			}

			fputc('[', m_targetFile);
			fputc(LogLevelToChar(level), m_targetFile);

			int64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
			int64_t timestampPart = timestamp / 1000000;
			int subtimestampPart = (int)(timestamp % 1000000);
			fprintf(m_targetFile, "] (%lld.%06d) \t", timestampPart, subtimestampPart);

			fwrite(logString.data(), sizeof(std::string_view::value_type), logString.length(), m_targetFile);
			fputc('\n', m_targetFile);
		}
	}
}
