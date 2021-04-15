////////////////////////////////////////////////////////////////////////////////
// WinPCPlatformApplication.cpp (Leggiero - Platform.WinPC)
//
// Implementation of Windows PC Platform Application
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinPCPlatformApplication.h"

// Leggiero.Engine
#include <Engine/Application/CreateGame.h>
#include <Engine/Application/IGame.h>

// Leggiero.Platform.WinPC
#include "Window/Window.h"
#include "Window/WindowCreateParam.h"
#include "OpenGL/EGLView.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			//////////////////////////////////////////////////////////////////////////////// WinPCPlatformApplication - Initialization

			//------------------------------------------------------------------------------
			WinPCPlatformApplication::WinPCPlatformApplication(HINSTANCE hInstance, int nCmdShow, LPCTSTR className, LPCTSTR windowName, HICON hIcon, HICON hIconSm, int screenWidth, int screenHeight, float simulatedXPPI, float simulatedYPPI)
				: m_hInstance(hInstance), m_isRunning(true)
				, m_windowPixelWidth(static_cast<Application::DeviceScreenPixelCoordType>(screenWidth)), m_windowPixelHeight(static_cast<Application::DeviceScreenPixelCoordType>(screenHeight))
				, m_windowWidth(static_cast<Application::DeviceScreenCoordType>(screenWidth)), m_windowHeight(static_cast<Application::DeviceScreenCoordType>(screenHeight))
				, m_approxScreenXPPI(static_cast<Application::DeviceScreenCoordType>(simulatedXPPI)), m_approxScreenYPPI(static_cast<Application::DeviceScreenCoordType>(simulatedYPPI))
				, m_safeAreaRect(static_cast<Application::DeviceScreenCoordType>(0.0f), static_cast<Application::DeviceScreenCoordType>(0.0f), static_cast<Application::DeviceScreenCoordType>(screenWidth), static_cast<Application::DeviceScreenCoordType>(screenHeight))
			{
				_InitializeApplicationComponent();

				_InitializeGameWindow(hInstance, nCmdShow, className, windowName, hIcon, hIconSm, screenWidth, screenHeight);
				_InitializeEGLView();

				_InitializeComponentAfterWindowCreation();
			}

			WinPCPlatformApplication::~WinPCPlatformApplication()
			{
				m_glView = nullptr;
			}

			void WinPCPlatformApplication::_InitializeGameWindow(HINSTANCE hInstance, int nCmdShow, LPCTSTR className, LPCTSTR windowName, HICON hIcon, HICON hIconSm, int windowWidth, int windowHeight)
			{
				WindowCreateParam windowParam;
				windowParam.className = className;

				windowParam.hInstance = hInstance;

				windowParam.nCmdShow = nCmdShow;

				windowParam.hIcon = hIcon;
				windowParam.hIconSm = hIconSm;

				windowParam.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

				windowParam.windowName = windowName;
				windowParam.windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER
					| WS_CLIPSIBLINGS | WS_CLIPCHILDREN; // for EGL

				windowParam.width = windowWidth;
				windowParam.height = windowHeight;
				windowParam.isSetContentsSize = true;

				m_gameWindow.reset(new Window(windowParam));
				m_gameWindow->SetMessageObserver(this);
			}

			void WinPCPlatformApplication::_InitializeEGLView()
			{
				m_glView.reset(new Graphics::EGLView(m_gameWindow->GetHWND()));
			}


			//////////////////////////////////////////////////////////////////////////////// WinPCPlatformApplication - Run

			//------------------------------------------------------------------------------
			// Application Run Function contains the Main Loop
			void WinPCPlatformApplication::Run(IGameLaunchOption *gameLaunchOption)
			{
				std::shared_ptr<IGame> game = CreateGame(this, gameLaunchOption);

				// Game Loop
				game->Prepare();
				game->GraphicPrepare();
				while (game->IsRunning() && m_isRunning)
				{
					// Process Game Frame
					bool isFrameProcessed = game->UpdateFrame();

					if (isFrameProcessed)
					{
						// Do Render
						m_glView->SwapBuffers();
					}

					Sleep(0);
				}
				game->GraphicShutdown();
				game->Shutdown();
			}


			//////////////////////////////////////////////////////////////////////////////// WinPCPlatformApplication - Windows Event Process

			//------------------------------------------------------------------------------
			bool WinPCPlatformApplication::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				GameTimeClockType::time_point eventTime = GameTimeClockType::now();
				if (_ProcessApplicationComponentWindowsMessage(message, wParam, lParam, eventTime))
				{
					return true;
				}
				return false;
			}

			//------------------------------------------------------------------------------
			void WinPCPlatformApplication::OnDestoryed(HWND hWnd)
			{
				// Shutdown the application when game window destroyed
				m_isRunning = false;
			}

			//------------------------------------------------------------------------------
			bool WinPCPlatformApplication::_ProcessApplicationComponentWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam, GameTimeClockType::time_point eventTime)
			{
				switch (message)
				{
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_MOUSEMOVE:
						{
							return _ProcessWindowsMouseMessage(message, wParam, lParam, eventTime);
						}

					case WM_TOUCH:
						{
							return _ProcessWindowsTouchMessage(message, wParam, lParam, eventTime);
						}
				}
				return false;
			}
		}
	}
}
