////////////////////////////////////////////////////////////////////////////////
// Service/WinWebView.h (Leggiero - Platform.WinPC)
//
// Windows Platform Web View Service
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Application
#include <Application/IAppWebViewController.h>

// Leggiero.Platform.WinPC
#include "../WindowsCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			// Web View Service
			class WinWebView
				: public Application::IAppWebViewController
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				WinWebView() { }
				virtual ~WinWebView() { }

			public:	// IAppWebViewController
				// Open URL in a Page
				virtual void OpenURLByPage(const char *pageTitle, const char *buttonCaption, const char *url) override;

				// Not an embeded page, so ignore check
				virtual bool IsURLPageOpened() override { return false; }

				// Open URL
				virtual void OpenURL(const char *url) override;

				// Check given URL Scheme is openable or not
				virtual bool CanOpenURL(const char *urlScheme) override;
			};
		}
	}
}
