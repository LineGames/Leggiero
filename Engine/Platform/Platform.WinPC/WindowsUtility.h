////////////////////////////////////////////////////////////////////////////////
// WindowsUtility.h (Leggiero - Platform.WinPC)
//
// Windows Platform Utility Functions
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <vector>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Platform.WinPC
#include "WindowsCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace Utility
			{
				GameDataString TSTR_2_GameDataString(LPCTSTR sourceString);
				GameDataString WSTR_2_GameDataString(LPCWSTR sourceString);

				std::vector<wchar_t> GameDataString_2_WSTR(const GameDataString &sourceString);

				void PrintDebugString(const char *str);
				void PrintDebugString(LPCWSTR str);

				void PrintDebugStringFormat(const char *fmt, ...);
				void PrintDebugStringFormat(const wchar_t *fmt, ...);
			}
		}
	}
}
