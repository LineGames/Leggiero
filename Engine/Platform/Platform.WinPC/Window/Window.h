////////////////////////////////////////////////////////////////////////////////
// Window/Window.h (Leggiero - Platform.WinPC)
//
// Abstraction for a Window in Windows System
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <exception>

// External Library
#include <pthread.h>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Platform.WinPC
#include "../WindowsCommon.h"
#include "WindowEventObserver.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			// Forward Declarations
			struct WindowCreateParam;
			class WindowCreateFailException;


			// Abstract Window Class
			class Window
			{
			public:
				Window(const WindowCreateParam &createParam)
					noexcept(false)
					: Window(createParam, NULL) { }

				Window(const WindowCreateParam &createParam, IWindowEventObserver *messageObserver)
					noexcept(false);

				virtual ~Window();

			public:
				HWND					GetHWND() const { return m_hWnd; }

				bool					IsValidWindow() const { return m_isValidWindow; }
				DWORD					GetWindowError() const { return m_lastError; }

				IWindowEventObserver	*GetMessageObserver() const { return m_messageObserver; }
				void					SetMessageObserver(IWindowEventObserver *observer) { m_messageObserver = observer; }

			private:
				HWND					m_hWnd;

				bool					m_isValidWindow;
				DWORD					m_lastError;

				bool					m_isTouchEnabled;

				IWindowEventObserver	*m_messageObserver;

				pthread_t				*m_messageLoopThread;
				HANDLE					m_createWaitEvent;

			private:
				static void *__PTW32_CDECL WindowThreadFunction(void *Args);

				static bool WindowRegisterClass(Window *_this, const WindowCreateParam &createParam);
				static bool WindowCreateWindow(Window *_this, const WindowCreateParam &createParam);

				static bool DisableWindowGestures(Window *_this);

				static void MessageLoopFunction(Window *_this);
			};
		}
	}
}
