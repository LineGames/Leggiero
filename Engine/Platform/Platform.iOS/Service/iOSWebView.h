////////////////////////////////////////////////////////////////////////////////
// Service/iOSWebView.h (Leggiero - Platform.iOS)
//
// iOS Platform Web View Service
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_IOS__SERVICE__IOS_WEB_VIEW_H
#define __LP_IOS__SERVICE__IOS_WEB_VIEW_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Application
#include <Application/IAppWebViewController.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			// Web View Service
			class IOSWebView
				: public Application::IAppWebViewController
				, private Leggiero::Utility::SyntacticSugar::NonCopyable
			{
			public:
                IOSWebView();
				virtual ~IOSWebView();

			public:	// IAppWebViewController
				// Open URL in a Page
				virtual void OpenURLByPage(const char *pageTitle, const char *buttonCaption, const char *url) override;
				
				// Check Whether URL Page is Opened or NOT
				virtual bool IsURLPageOpened() override;

				// Open URL
				virtual void OpenURL(const char *url) override;

				// Check given URL Scheme is openable or not
				virtual bool CanOpenURL(const char *urlScheme) override;
			};
		}
	}
}

#endif
