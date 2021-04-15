////////////////////////////////////////////////////////////////////////////////
// Window/WindowEventObserver.h (Leggiero - Platform.WinPC)
//
// Observer Interface to handle Window Message
////////////////////////////////////////////////////////////////////////////////

#pragma once


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
			// Observer Interface of Events from a Window
			class IWindowEventObserver
			{
			public:
				virtual ~IWindowEventObserver() { };

			public:
				// Handle a message
				// Should return true if the message have been handled.
				virtual bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return false; }

				// Notify when the message loop ends
				virtual void OnDestoryed(HWND hWnd) { }
			};
		}
	}
}
