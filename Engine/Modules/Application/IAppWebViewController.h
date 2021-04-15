////////////////////////////////////////////////////////////////////////////////
// IAppWebViewController.h (Leggiero/Modules - Application)
//
// Interface for Web View Service Controller Sub-Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__I_APP_WEB_VIEW_CONTROLLER_H
#define __LM_APPLICATION__I_APP_WEB_VIEW_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Application
	{
		// Web View Service Controller Interface
		class IAppWebViewController
		{
		public:
			// Open URL in a Page
			virtual void OpenURLByPage(const char *pageTitle, const char *buttonCaption, const char *url) = 0;

			// Check Whether URL Page is Opened or NOT
			virtual bool IsURLPageOpened() = 0;

			// Open URL
			virtual void OpenURL(const char *url) = 0;

			// Check given URL Scheme is openable or not
			virtual bool CanOpenURL(const char *urlScheme) = 0;
		};
	}
}

#endif
