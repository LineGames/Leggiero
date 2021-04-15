////////////////////////////////////////////////////////////////////////////////
// WinPCPlatformApplication.h (Leggiero - Platform.WinPC)
//
// Platform Application Object runs on Windows PC Platform
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <memory>
#include <string>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Engine
#include <Engine/Application/IPlatformApplication.h>

// Leggiero.Application
#include <Application/AppPlatformType.h>
#include <Application/DeviceCommonTypes.h>
#include <Application/ApplicationComponent.h>
#include <Application/IAppInformationProvider.h>
#include <Application/IDeviceInformationProvider.h>
#include <Application/IPlatformAppControl.h>
#include <Application/SystemEventDispatcher.h>
#include <Application/RawTouchEventDispatcher.h>

// Leggiero.Platform.WinPC
#include "WindowsCommon.h"
#include "Window/WindowEventObserver.h"


namespace Leggiero
{
	// Forward Declarations
	class IGameLaunchOption;


	namespace Platform
	{
		namespace Windows
		{
			// Forward Declarations
			class Window;
			class WinTextInputDialog;
			class WinWebView;

			namespace Graphics
			{
				class EGLView;
			}


			// Platform Application
			class WinPCPlatformApplication
				: public IWindowEventObserver
				, public IPlatformApplication
				, public Application::ApplicationComponent
				, public Application::IAppInformationProvider, public Application::IDeviceInformationProvider
				, public Application::IPlatformAppControl
				, public Application::SystemEventDispatcher, public Application::RawTouchEventDispatcher
			{
			public:
				WinPCPlatformApplication(HINSTANCE hInstance, int nCmdShow, LPCTSTR className, LPCTSTR windowName, HICON hIcon, HICON hIconSm, int screenWidth, int screenHeight, float simulatedXPPI, float simulatedYPPI)
					noexcept(false);
				virtual ~WinPCPlatformApplication();

			public:
				// Application Run Function contains the Main Loop
				void Run(IGameLaunchOption *gameLaunchOption);

				// Set Application Meta-Data
				void SetIsTablet(bool isTablet);
				void SetSystemLocale(const std::string &systemLocaleString);
				void SetReferrer(const std::string &appLaunchReferrer);
				void SetSafePadding(float paddingLeft, float paddingTop, float paddingRight, float paddingBottom);

			public:	// Reference Constants
				// Touch Id of Mouse Cursor
				static const Application::DeviceTouchIdType s_kMouseTouchId;

			public:	// IWindowEventObserver
				virtual bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
				virtual void OnDestoryed(HWND hWnd) override;

			public:	// IPlatformApplication
				virtual Application::ApplicationComponent *GetApplicationComponent() override { return this; }
				virtual EngineComponent *GetApplicationEngineComponent() override { return GetApplicationComponent(); }

			public:	// ApplicationComponent
				virtual Application::IAppInformationProvider &AppInformation() override { return *this; }
				virtual Application::IDeviceInformationProvider &DeviceInformation() override { return *this; }

				virtual Application::IPlatformAppControl &PlatformAppControl() override { return *this; }

				virtual Application::SystemEventDispatcher &SystemEventCenter() override { return *this; }
				virtual Application::RawTouchEventDispatcher &RawTouchEventCenter() override { return *this; }

				virtual Application::IAppTextInputController &TextInputController() override;
				virtual Application::IAppWebViewController &WebViewController() override;

			public:	// IAppInformationProvider
				virtual Application::DeviceScreenCoordType GetPixelWidth() const override { return m_windowWidth; }
				virtual Application::DeviceScreenCoordType GetPixelHeight() const override { return m_windowHeight; }
				virtual Application::DeviceScreenPixelCoordType GetIntegerPixelWidth() const override { return m_windowPixelWidth; }
				virtual Application::DeviceScreenPixelCoordType GetIntegerPixelHeight() const override { return m_windowPixelHeight; }

				virtual Application::AppPlatformType GetPlatformType() const override { return Application::AppPlatformType::kWinPC; }

				virtual const char *GetSystemLocaleString() const override { return m_systemLocaleString.c_str(); }

				virtual std::string GetAppReferrer() override { return m_appLaunchReferrer; }

			public:	// IDeviceInformationProvider
				virtual Application::DeviceScreenCoordType GetScreenPixelWidth() const override { return m_windowWidth; }
				virtual Application::DeviceScreenCoordType GetScreenPixelHeight() const override { return m_windowHeight; }
				virtual Application::DeviceScreenPixelCoordType GetScreenIntegerPixelWidth() const override { return m_windowPixelWidth; }
				virtual Application::DeviceScreenPixelCoordType GetScreenIntegerPixelHeight() const override { return m_windowPixelHeight; }

				virtual Application::DeviceScreenCoordType GetApproximateScreenXPPI() const override { return m_approxScreenXPPI; }
				virtual Application::DeviceScreenCoordType GetApproximateScreenYPPI() const override { return m_approxScreenYPPI; }

				virtual Utility::Math::BasicRect<Application::DeviceScreenCoordType> GetSafeArea() const override { return m_safeAreaRect; }

				virtual GameDataString GetDeviceName() const override { return "WinPC x64"; }
				virtual bool IsTablet() const override { return m_isTablet; }

			public:	// IPlatformAppControl
				// Do nothing in WinPC Platform
				virtual void SetNoSleepMode(bool isPreventSleep) { }

			protected:
				HINSTANCE							m_hInstance;
				std::shared_ptr<Window>				m_gameWindow;
				std::shared_ptr<Graphics::EGLView>	m_glView;

				bool						m_isRunning;

				Application::DeviceScreenPixelCoordType m_windowPixelWidth;
				Application::DeviceScreenPixelCoordType m_windowPixelHeight;
				Application::DeviceScreenCoordType		m_windowWidth;
				Application::DeviceScreenCoordType		m_windowHeight;

				Application::DeviceScreenCoordType		m_approxScreenXPPI;
				Application::DeviceScreenCoordType		m_approxScreenYPPI;

				Utility::Math::BasicRect<Application::DeviceScreenCoordType> m_safeAreaRect;

				bool			m_isTablet;
				std::string		m_systemLocaleString;
				std::string		m_appLaunchReferrer;

				bool m_isMouseDowned;

				std::shared_ptr<WinTextInputDialog> m_textInputDialogController;
				std::shared_ptr<WinWebView> m_webViewController;

			private:
				void _InitializeGameWindow(HINSTANCE hInstance, int nCmdShow, LPCTSTR className, LPCTSTR windowName, HICON hIcon, HICON hIconSm, int windowWidth, int windowHeight)
					noexcept(false);
				void _InitializeEGLView()
					noexcept(false);

				void _InitializeApplicationComponent();
				void _InitializeComponentAfterWindowCreation();

			protected:
				virtual bool _ProcessApplicationComponentWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam, GameTimeClockType::time_point eventTime);

				virtual bool _ProcessWindowsMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, GameTimeClockType::time_point eventTime);
				virtual bool _ProcessWindowsTouchMessage(UINT message, WPARAM wParam, LPARAM lParam, GameTimeClockType::time_point eventTime);
			};
		}
	}
}
