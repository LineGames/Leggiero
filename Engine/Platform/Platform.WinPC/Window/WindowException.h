////////////////////////////////////////////////////////////////////////////////
// Window/WindowException.h (Leggiero - Platform.WinPC)
//
// Exceptions related to a Window
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <exception>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Platform.WinPC
#include "../WindowsCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			// Window Cretation Failed Exception
			class WindowCreateFailException : public std::exception
			{
			public:
				WindowCreateFailException()
					: WindowCreateFailException(false, false, ERROR_SUCCESS) { }

				WindowCreateFailException(
					bool isClassRegisterFailed, bool isWindowCreateFailed,
					DWORD lastError);

				virtual ~WindowCreateFailException() throw() { }

			public:
				virtual const char *what() const throw() { return "Failed to create window"; }

			public:
				bool	isClassRegisterFailed;
				bool	isWindowCreateFailed;

				DWORD	lastError;
			};
		}
	}
}
