////////////////////////////////////////////////////////////////////////////////
// DebugLogger.cpp (Leggiero/Modules - Log)
//
// Debug Logger Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "DebugLogger.h"
#include "_Internal/_DebugLoggerInterface.h"

// Leggiero.Log
#include "PlatformDefaultLogWriter.h"



namespace Leggiero
{
	namespace Log
	{
		//////////////////////////////////////////////////////////////////////////////// DebugLogger

		//------------------------------------------------------------------------------
		namespace _Internal
		{
			DebugLogger g_debugLogger;
		}

#ifdef _LEGGIERO_DEBUG

		//------------------------------------------------------------------------------
		DebugLogger &GetDebugLogger()
		{
			return _Internal::g_debugLogger;
		}

#endif


		//////////////////////////////////////////////////////////////////////////////// DebugLogger Module Initialization

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			void _InitializeDebugLogger()
			{
#ifdef _LEGGIERO_DEBUG
				_Internal::g_debugLogger.RegisterLogWriter(std::make_shared<PlatformDefaultLogWriter>(LogLevelType::kTrace, LogLevelType::kNoLog, false));
#endif
			}

			//------------------------------------------------------------------------------
			void _FinalizeDebugLogger()
			{
			}
		}
	}
}
