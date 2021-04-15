////////////////////////////////////////////////////////////////////////////////
// Service/WinWebView.cpp (Leggiero - Platform.WinPC)
//
// Windows Web View Service Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinWebView.h"

// External Library
#include <shellapi.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			//////////////////////////////////////////////////////////////////////////////// WinWebView

			//------------------------------------------------------------------------------
			// Open URL in a Page
			// In Windows, just open as ShellExecute
			void WinWebView::OpenURLByPage(const char *pageTitle, const char *buttonCaption, const char *url)
			{
				ShellExecuteA(0, 0, url, 0, 0, SW_SHOW);
			}

			//------------------------------------------------------------------------------
			// Open URL
			void WinWebView::OpenURL(const char *url)
			{
				ShellExecuteA(0, 0, url, 0, 0, SW_SHOW);
			}

			//------------------------------------------------------------------------------
			// Check given URL Scheme is openable or not
			bool WinWebView::CanOpenURL(const char *urlScheme)
			{
				return true;
			}
		}
	}
}
