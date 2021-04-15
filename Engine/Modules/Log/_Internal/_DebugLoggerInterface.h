////////////////////////////////////////////////////////////////////////////////
// _Internal/_DebugLoggerInterface.h (Leggiero/Modules - Log)
//   * DO NOT directly include this header file outside of Log project
//
// Debug Logger related Internal Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LOG___DEBUG_LOGGER_INTERFACE_H
#define __LM_LOG___DEBUG_LOGGER_INTERFACE_H


namespace Leggiero
{
	namespace Log
	{
		namespace _Internal
		{
			void _InitializeDebugLogger();
			void _FinalizeDebugLogger();
		}
	}
}

#endif
