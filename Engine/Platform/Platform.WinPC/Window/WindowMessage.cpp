////////////////////////////////////////////////////////////////////////////////
// Window/WindowMessage.cpp (Leggiero - Platform.WinPC)
//
// Implementation of Windows message handling functions
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WindowMessage.h"

// External Library
#include <tpcshrd.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Platform.WinPC
#include "Window.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			//------------------------------------------------------------------------------
			LRESULT CALLBACK WindowMessageCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
#ifdef LEGGIERO_OPTIMIZE_WINDOW_ONLYONE
				const Window *messageOwner = _messageOwnerWindow;
#else
				const Window *messageOwner = WindowManager::GetInstance().FindMessageWindow(hWnd);
#endif
				if (messageOwner != NULL && messageOwner->IsValidWindow())
				{
					IWindowEventObserver *observer = messageOwner->GetMessageObserver();
					if (observer != NULL)
					{
						if (observer->HandleMessage(hWnd, message, wParam, lParam))
						{
							return 0;
						}
					}
				}

				// Default Message Process
				switch (message)
				{
				case WM_DESTROY:
					PostQuitMessage(0);
					break;

					// For game-friendly touch process
				case WM_TABLET_QUERYSYSTEMGESTURESTATUS:
				{
					return (TABLET_DISABLE_PRESSANDHOLD
						| TABLET_DISABLE_PENTAPFEEDBACK
						| TABLET_DISABLE_PENBARRELFEEDBACK
						| TABLET_DISABLE_TOUCHUIFORCEON
						| TABLET_DISABLE_TOUCHUIFORCEOFF
						| TABLET_DISABLE_TOUCHSWITCH
						| TABLET_DISABLE_FLICKS
						| TABLET_DISABLE_SMOOTHSCROLLING
						| TABLET_DISABLE_FLICKFALLBACKKEYS
						| TABLET_ENABLE_MULTITOUCHDATA);
				}

				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				return 0;
			}


#ifdef LEGGIERO_OPTIMIZE_WINDOW_ONLYONE
			Window *_messageOwnerWindow = NULL;
#else
			//////////////////////////////////////////////////////////////////////////////// WindowManager

			//------------------------------------------------------------------------------
			void WindowManager::Initialize()
			{
				InitializeSRWLock(&windowTableLock);
			}

			//------------------------------------------------------------------------------
			const Window *WindowManager::FindMessageWindow(HWND hWnd)
			{
				{
					PSRWLOCK lockPointer = &windowTableLock;
					auto unlockFunc = [lockPointer] { ReleaseSRWLockShared(lockPointer); };
					FINALLY_OF_BLOCK(_UNLOCK_TABLE, unlockFunc);

					AcquireSRWLockShared(lockPointer);

					std::map<HWND, const Window *>::const_iterator findIt = windowTable.find(hWnd);
					if (findIt == windowTable.end())
					{
						return NULL;
					}
					return findIt->second;
				}
			}

			//------------------------------------------------------------------------------
			void WindowManager::RegisterWindow(const Window &window)
			{
				{
					PSRWLOCK lockPointer = &windowTableLock;
					auto unlockFunc = [lockPointer] { ReleaseSRWLockExclusive(lockPointer); };
					FINALLY_OF_BLOCK(_UNLOCK_TABLE, unlockFunc);

					AcquireSRWLockExclusive(lockPointer);

					HWND windowHWnd = window.GetHWND();
					std::map<HWND, const Window *>::const_iterator findIt = windowTable.find(windowHWnd);
					if (findIt != windowTable.end())
					{
						// Already registered
						return;
					}
					windowTable[windowHWnd] = &window;
				}
			}

			//------------------------------------------------------------------------------
			void WindowManager::UnRegisterWindow(const Window &window)
			{
				{
					PSRWLOCK lockPointer = &windowTableLock;
					auto unlockFunc = [lockPointer] { ReleaseSRWLockExclusive(lockPointer); };
					FINALLY_OF_BLOCK(_UNLOCK_TABLE, unlockFunc);

					AcquireSRWLockExclusive(lockPointer);

					windowTable.erase(window.GetHWND());
				}
			}
#endif
		}
	}
}

#ifndef LEGGIERO_OPTIMIZE_WINDOW_ONLYONE
LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::Platform::Windows::WindowManager);
#endif
