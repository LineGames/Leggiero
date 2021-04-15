////////////////////////////////////////////////////////////////////////////////
// WinPCApplicationComponent.cpp (Leggiero - Platform.WinPC)
//
// Implementation of Application Component Functions in Windows PC Platform
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinPCPlatformApplication.h"

// External Library
#include <utfcpp/utf8.h>

// Leggiero.Platform.WinPC
#include "Window/Window.h"
#include "Service/WinTextInputDialog.h"
#include "Service/WinWebView.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			//////////////////////////////////////////////////////////////////////////////// WinPCPlatformApplication

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::_InitializeApplicationComponent()
			{
				// Default Meta Values
				m_isTablet = false;
				m_systemLocaleString = "en_US";
				m_appLaunchReferrer = "";

				// Sub-Components
				m_webViewController = std::make_shared<WinWebView>();
			}

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::_InitializeComponentAfterWindowCreation()
			{
				m_textInputDialogController = std::make_shared<WinTextInputDialog>(m_hInstance, m_gameWindow->GetHWND());
			}

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::SetIsTablet(bool isTablet)
			{
				m_isTablet = isTablet;
			}

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::SetSystemLocale(const std::string &systemLocaleString)
			{
				m_systemLocaleString = systemLocaleString;
			}

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::SetReferrer(const std::string &appLaunchReferrer)
			{
				m_appLaunchReferrer = appLaunchReferrer;
			}

			//------------------------------------------------------------------------------
			Application::IAppTextInputController &WinPCPlatformApplication::TextInputController()
			{
				return *m_textInputDialogController;
			}

			//------------------------------------------------------------------------------
			Application::IAppWebViewController &WinPCPlatformApplication::WebViewController()
			{
				return *m_webViewController;
			}

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::SetSafePadding(float paddingLeft, float paddingTop, float paddingRight, float paddingBottom)
			{
				m_safeAreaRect.left = static_cast<Application::DeviceScreenCoordType>(paddingLeft);
				m_safeAreaRect.top = static_cast<Application::DeviceScreenCoordType>(paddingTop);
				m_safeAreaRect.right = m_windowWidth - static_cast<Application::DeviceScreenCoordType>(paddingRight);
				m_safeAreaRect.bottom = m_windowHeight - static_cast<Application::DeviceScreenCoordType>(paddingBottom);
			}
		}
	}
}
