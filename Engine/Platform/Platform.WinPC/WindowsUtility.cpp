////////////////////////////////////////////////////////////////////////////////
// WindowsUtility.cpp (Leggiero - Platform.WinPC)
//
// Implementation of Windows Platform Utility Functions
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WindowsUtility.h"

// Standrad Library
#include <cstdio>
#include <cstdarg>
#include <cwchar>
#include <vector>

// External Library
#include <utfcpp/utf8.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			namespace Utility
			{
				//////////////////////////////////////////////////////////////////////////////// String Conversion

				//------------------------------------------------------------------------------
				GameDataString TSTR_2_GameDataString(LPCTSTR sourceString)
				{
					if (sizeof(TCHAR) == 2)
					{
						// UTF-16 wchar
						return WSTR_2_GameDataString((LPCWSTR)sourceString);
					}
					else if (sizeof(TCHAR) == 1)
					{
						// ANSI
						return GameDataString((const char *)sourceString);
					}

					// not considered case until now
					return "<INVALID T-STRING>";
				}

				//------------------------------------------------------------------------------
				GameDataString WSTR_2_GameDataString(LPCWSTR sourceString)
				{
					size_t stringLength = wcslen(sourceString);

					std::vector<char> buf;
					utf8::utf16to8(sourceString, sourceString + stringLength, std::back_inserter(buf));
					buf.push_back('\0');

					return GameDataString(&(buf[0]));
				}

				//------------------------------------------------------------------------------
				std::vector<wchar_t> GameDataString_2_WSTR(const GameDataString &sourceString)
				{
					size_t sourceLength = sourceString.length();
					std::vector<wchar_t> utf16Result;
					utf8::utf8to16(sourceString.cbegin(), sourceString.cend(), std::back_inserter(utf16Result));
					utf16Result.push_back(L'\0');

					return utf16Result;
				}


				//////////////////////////////////////////////////////////////////////////////// Debug

				//------------------------------------------------------------------------------
				void PrintDebugString(const char *str)
				{
					// Check UTF-8 String
					bool isSpecialCharExist = false;
					size_t strByteLength = strnlen_s(str, 65536);	// May be enough for debug string
					for (int i = 0; i < strByteLength; ++i)
					{
						if ((str[i] & 0x80) != 0)
						{
							isSpecialCharExist = true;
							break;
						}
					}

					if (isSpecialCharExist)
					{
						if (utf8::is_valid(str, str + strByteLength))
						{
							// In Windows platform, unicode character should be converted into UTF-16 wchar
							std::vector<wchar_t> utf16Result;
							utf8::utf8to16(str, str + strByteLength, std::back_inserter(utf16Result));
							utf16Result.push_back(L'\0');

							OutputDebugStringW(&(utf16Result[0]));
							return;
						}
					}

					// Print as-is
					OutputDebugStringA(str);
				}

				//------------------------------------------------------------------------------
				void PrintDebugString(LPCWSTR str)
				{
					OutputDebugStringW(str);
				}

				//------------------------------------------------------------------------------
				void PrintDebugStringFormat(const char *fmt, ...)
				{
					const size_t MAX_LOG_SIZE = 4096;

					char buf[MAX_LOG_SIZE];

					va_list args;
					va_start(args, fmt);
					vsprintf_s(buf, MAX_LOG_SIZE, fmt, args);
					va_end(args);

					PrintDebugString(buf);
				}

				//------------------------------------------------------------------------------
				void PrintDebugStringFormat(const wchar_t *fmt, ...)
				{
					const size_t MAX_LOG_SIZE = 4096;

					wchar_t buf[MAX_LOG_SIZE];

					va_list args;
					va_start(args, fmt);
					vswprintf_s(buf, MAX_LOG_SIZE, fmt, args);
					va_end(args);

					OutputDebugStringW(buf);
				}
			}
		}
	}
}
