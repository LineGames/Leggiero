////////////////////////////////////////////////////////////////////////////////
// Service/AndroidWebView.h (Leggiero - Platform.Android)
//
// Android Platform Web View Service
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_ANDROID__SERVICE__ANDROID_WEB_VIEW_H
#define __LP_ANDROID__SERVICE__ANDROID_WEB_VIEW_H


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
		namespace Android
		{
			// Web View Service
			class AndroidWebView
				: public Application::IAppWebViewController
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				AndroidWebView();
				virtual ~AndroidWebView();

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
