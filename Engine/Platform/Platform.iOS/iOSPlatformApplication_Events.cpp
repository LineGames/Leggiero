////////////////////////////////////////////////////////////////////////////////
// iOSPlatformApplication_Events.cpp (Leggiero - Platform.iOS)
//
// Implementation of iOS Platform Application's System Event Handling Parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSPlatformApplication.h"

// Leggiero.Application
#include <Application/SystemEventObserver.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			//////////////////////////////////////////////////////////////////////////////// IOSPlatformApplication - System Events

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::OnPause()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnPause();
					});
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::OnResume()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnResume();
					});
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::OnGoToBackground()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnGoToBackground();
					});
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::OnReturnFromBackground()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnReturnFromBackground();
					});
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::OnMemoryWarning()
			{
				m_systemEventNotifier.NotifyEvent(
					[](Application::ISystemEventObserver *observer) mutable
					{
						observer->OnMemoryWarning();
					});
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::OnBackButtonPressed()
			{
				m_commandEventNotifier.NotifyEvent(
					[](Application::ISystemCommandEventObserver *observer) mutable
					{
						observer->OnBackButtonPressed();
					});
			}
		}
	}
}
