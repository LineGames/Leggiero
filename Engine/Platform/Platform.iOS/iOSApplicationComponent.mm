////////////////////////////////////////////////////////////////////////////////
// iOSApplicationComponent.mm (Leggiero - Platform.iOS)
//
// Implementation of Application Component Functions in iOS Platform
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSPlatformApplication.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Engine
#include <Engine/Application/CreateGame.h>
#include <Engine/Application/IGame.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Platform.iOS
#include "Service/iOSTextInputDialog.h"
#include "Service/iOSWebView.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			//////////////////////////////////////////////////////////////////////////////// IOSPlatformApplication

			//------------------------------------------------------------------------------
			std::string IOSPlatformApplication::GetAppReferrer()
			{
                return "";
			}

			//------------------------------------------------------------------------------
			GameDataString IOSPlatformApplication::GetDeviceName() const
			{
                return "";
			}


			//////////////////////////////////////////////////////////////////////////////// IOSPlatformApplication - Services

			//------------------------------------------------------------------------------
			Application::IAppTextInputController &IOSPlatformApplication::TextInputController()
			{
				return *m_textInputDialogController;
			}

			//------------------------------------------------------------------------------
			Application::IAppWebViewController &IOSPlatformApplication::WebViewController()
			{
				return *m_webViewController;
			}
		}
	}
}
