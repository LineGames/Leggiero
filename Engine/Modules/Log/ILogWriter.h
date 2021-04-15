////////////////////////////////////////////////////////////////////////////////
// ILogWriter.h (Leggiero/Modules - Log)
//
// Interface for Log Writer
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG__I_LOG_WRITER_H
#define __LM_LOG__I_LOG_WRITER_H


// Standard Library
#include <string_view>

// Leggiero.Log
#include "LogTypes.h"


namespace Leggiero
{
	namespace Log
	{
		// Log Writer Interface
		class ILogWriter
		{
		public:
			virtual ~ILogWriter() { }

		public:
			virtual void WriteLog(LogLevelType level, LogTime time, std::string_view logString) = 0;
		};
	}
}

#endif
