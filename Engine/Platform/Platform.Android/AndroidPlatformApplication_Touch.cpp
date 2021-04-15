////////////////////////////////////////////////////////////////////////////////
// AndroidPlatformApplication_Touch.cpp (Leggiero - Platform.Android)
//
// Implementation of Android Platform Application's Touch Related Parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidPlatformApplication.h"

// Leggiero.Application
#include <Application/RawTouchEventObserver.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidPlatformApplication - Touch

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnTouchDown(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType xPos, Application::DeviceScreenCoordType yPos, GameTimeClockType::time_point eventTime)
			{
				m_downedTouches.insert(touchId);

				m_touchEventNotifier.NotifyEvent(
					[touchId, xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
					{
						observer->OnTouchDown(touchId, xPos, yPos, eventTime);
					});
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnTouchMove(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType xPos, Application::DeviceScreenCoordType yPos, GameTimeClockType::time_point eventTime)
			{
				if (m_downedTouches.find(touchId) != m_downedTouches.end())
				{
					m_touchEventNotifier.NotifyEvent(
						[touchId, xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
						{
							observer->OnTouchMoved(touchId, xPos, yPos, eventTime);
						});
				}
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::OnTouchUp(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType xPos, Application::DeviceScreenCoordType yPos, GameTimeClockType::time_point eventTime)
			{
				if (m_downedTouches.find(touchId) != m_downedTouches.end())
				{
					m_downedTouches.erase(touchId);

					m_touchEventNotifier.NotifyEvent(
						[touchId, xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
						{
							observer->OnTouchUp(touchId, xPos, yPos, eventTime);
						});
				}
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::CancelAllTouch()
			{
				GameTimeClockType::time_point eventTime = GameTimeClockType::now();
				std::for_each(m_downedTouches.begin(), m_downedTouches.end(),
					[this, eventTime](Application::DeviceTouchIdType touchId) mutable
					{
						m_touchEventNotifier.NotifyEvent(
							[touchId, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
							{
								observer->OnTouchCanceled(touchId, eventTime);
							});
					});
				m_downedTouches.clear();
			}
		}
	}
}
