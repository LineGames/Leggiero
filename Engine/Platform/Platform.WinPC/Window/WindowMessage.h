////////////////////////////////////////////////////////////////////////////////
// Window/WindowMessage.h (Leggiero - Platform.WinPC)
//
// Functions to handle Windows message from a Window
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <map>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/SingletonPattern.h>

// Leggiero.Platform.WinPC
#include "../WindowsCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			// Forward Declaration
			class Window;


			// Callback Function to Handle Messages
			LRESULT CALLBACK WindowMessageCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef LEGGIERO_OPTIMIZE_WINDOW_ONLYONE
			extern Window *_messageOwnerWindow;
#else
			// Manager Class for Windows - can dispatch messages to each Windows
			class WindowManager
				: public Utility::DesignPattern::Singleton<WindowManager>
			{
				LEGGIERO_MAKE_SINGLETON_UNIQUE(WindowManager);

			private:
				virtual void Initialize();

			public:
				const Window	*FindMessageWindow(HWND hWnd);

				void			RegisterWindow(const Window &window);
				void			UnRegisterWindow(const Window &window);

			private:
				std::map<HWND, const Window *>	windowTable;
				SRWLOCK							windowTableLock;
			};
#endif
		}
	}
}
