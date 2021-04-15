////////////////////////////////////////////////////////////////////////////////
// Window/WindowCreateParam.h (Leggiero - Platform.WinPC)
//
// Window Create Parameter Structure Definition
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
			// Structure contains window creation option
			struct WindowCreateParam
			{
				HINSTANCE	hInstance;
				HWND		hWndParent;

				LPCTSTR		className;
				LPCTSTR		windowName;

				int			nCmdShow;

				LPCTSTR		menuName;
				HMENU		hMenu;

				UINT		classStyle;
				DWORD		windowStyle;

				int			x;
				int			y;
				int			width;
				int			height;
				bool		isSetContentsSize;

				HBRUSH		hbrBackground;
				HICON		hIcon;
				HICON		hIconSm;
				HCURSOR		hCursor;

				LPVOID		lpParam;

				WindowCreateParam()
					: hInstance(NULL)
					, hWndParent(NULL)
					, className(L"LeggieroDefaultWin")
					, windowName(L"")
					, nCmdShow(SW_SHOWNORMAL)
					, menuName(NULL), hMenu(NULL)
					, classStyle(0), windowStyle(WS_OVERLAPPEDWINDOW)
					, x(CW_USEDEFAULT), y(CW_USEDEFAULT)
					, width(CW_USEDEFAULT), height(CW_USEDEFAULT), isSetContentsSize(false)
					, hbrBackground(NULL)
					, hIcon(NULL), hIconSm(NULL)
					, hCursor(NULL)
					, lpParam(NULL)
				{
				}
			};
		}
	}
}
