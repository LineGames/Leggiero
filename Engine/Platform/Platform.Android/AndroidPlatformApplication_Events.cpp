////////////////////////////////////////////////////////////////////////////////
// AndroidPlatformApplication_Events.cpp (Leggiero - Platform.Android)
//
// Implementation of Android Platform Application's System Event Handling Parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidPlatformApplication.h"

// Leggiero.Application
#include <Application/SystemEventObserver.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidPlatformApplication - System Events

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnPause()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnPause();
					});
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnResume()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnResume();
					});
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnGoToBackground()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnGoToBackground();
					});
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnReturnFromBackground()
			{
				m_applicationEventNotifier.NotifyEvent(
					[](Application::IApplicationEventObserver *observer) mutable
					{
						observer->OnReturnFromBackground();
					});
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnMemoryWarning()
			{
				m_systemEventNotifier.NotifyEvent(
					[](Application::ISystemEventObserver *observer) mutable
					{
						observer->OnMemoryWarning();
					});
			}

			//------------------------------------------------------------------------------
			bool AndroidPlatformApplication::OnBackButtonPressed()
			{
				bool isHandled = m_isDefaultBackButtonHandling;
				m_commandEventNotifier.NotifyEvent(
					[&isHandled](Application::ISystemCommandEventObserver *observer) mutable
					{
						isHandled |= observer->OnBackButtonPressed();
					});
				return isHandled;
			}
		}
	}
}
