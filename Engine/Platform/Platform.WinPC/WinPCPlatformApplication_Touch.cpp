////////////////////////////////////////////////////////////////////////////////
// WinPCPlatformApplication.cpp (Leggiero - Platform.WinPC)
//
// Implementation of Windows PC Platform Application's Touch Event Process
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinPCPlatformApplication.h"

// Leggiero.Application
#include <Application/RawTouchEventObserver.h>

// Leggiero.Platform.WinPC
#include "Window/Window.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			//////////////////////////////////////////////////////////////////////////////// WinPCPlatformApplication - Touch Event Process

			//------------------------------------------------------------------------------
			const Application::DeviceTouchIdType WinPCPlatformApplication::s_kMouseTouchId = 0;

			//------------------------------------------------------------------------------
			bool WinPCPlatformApplication::_ProcessWindowsMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, GameTimeClockType::time_point eventTime)
			{
				// Parse Parameters
				Application::DeviceScreenCoordType xPos = static_cast<Application::DeviceScreenCoordType>(GET_X_LPARAM(lParam));
				Application::DeviceScreenCoordType yPos = static_cast<Application::DeviceScreenCoordType>(GET_Y_LPARAM(lParam));

				// Notify to Observers
				switch (message)
				{
					case WM_LBUTTONDOWN:
						{
							m_isMouseDowned = true;
							m_touchEventNotifier.NotifyEvent(
								[xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
								{
									observer->OnTouchDown(s_kMouseTouchId, xPos, yPos, eventTime);
								});
						}
						break;

					case WM_LBUTTONUP:
						{
							if (m_isMouseDowned)
							{
								m_isMouseDowned = false;
								m_touchEventNotifier.NotifyEvent(
									[xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
									{
										observer->OnTouchUp(s_kMouseTouchId, xPos, yPos, eventTime);
									});
							}
						}
						break;

					case WM_MOUSEMOVE:
						{
							if (m_isMouseDowned)
							{
								m_touchEventNotifier.NotifyEvent(
									[xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
									{
										observer->OnTouchMoved(s_kMouseTouchId, xPos, yPos, eventTime);
									});
							}
						}
						break;
				}

				return true;
			}

			//------------------------------------------------------------------------------
			bool WinPCPlatformApplication::_ProcessWindowsTouchMessage(UINT message, WPARAM wParam, LPARAM lParam, GameTimeClockType::time_point eventTime)
			{
				UINT cInputs = LOWORD(wParam);
				PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
				if (pInputs != nullptr)
				{
					if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT)))
					{
						for (UINT i = 0; i < cInputs; i++)
						{
							TOUCHINPUT ti = pInputs[i];
							if (ti.dwID == s_kMouseTouchId)
							{
								// Ignore touch with same Id as Mouse
								continue;
							}
							Application::DeviceTouchIdType touchId = static_cast<Application::DeviceTouchIdType>(ti.dwID);

							POINT pt;
							pt.x = TOUCH_COORD_TO_PIXEL(ti.x);
							pt.y = TOUCH_COORD_TO_PIXEL(ti.y);
							ScreenToClient(m_gameWindow->GetHWND(), (LPPOINT)&pt);
							Application::DeviceScreenCoordType xPos = static_cast<Application::DeviceScreenCoordType>(pt.x);
							Application::DeviceScreenCoordType yPos = static_cast<Application::DeviceScreenCoordType>(pt.y);

							if ((ti.dwFlags & TOUCHEVENTF_DOWN) != 0)
							{
								m_touchEventNotifier.NotifyEvent(
									[touchId, xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
									{
										observer->OnTouchDown(touchId, xPos, yPos, eventTime);
									});
							}
							if ((ti.dwFlags & TOUCHEVENTF_MOVE) != 0)
							{
								m_touchEventNotifier.NotifyEvent(
									[touchId, xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
									{
										observer->OnTouchMoved(touchId, xPos, yPos, eventTime);
									});
							}
							if ((ti.dwFlags & TOUCHEVENTF_UP) != 0)
							{
								m_touchEventNotifier.NotifyEvent(
									[touchId, xPos, yPos, eventTime](Application::IRawTouchInputEventObserver *observer) mutable
									{
										observer->OnTouchUp(touchId, xPos, yPos, eventTime);
									});
							}
						}
					}
					else
					{
						// Event Error Occured
					}
					CloseTouchInputHandle((HTOUCHINPUT)lParam);
					delete[] pInputs;
				}
				else
				{
					// Out of memory??
				}

				return true;
			}
		}
	}
}
