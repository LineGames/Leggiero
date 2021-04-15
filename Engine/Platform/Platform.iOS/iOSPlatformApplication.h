////////////////////////////////////////////////////////////////////////////////
// iOSPlatformApplication.h (Leggiero - Platform.iOS)
//
// Platform Application Object runs on iOS Platform
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_IOS__IOS_PLATFORM_APPLICATION_H
#define __LP_IOS__IOS_PLATFORM_APPLICATION_H


// Standard Library
#include <memory>
#include <string>
#include <unordered_set>

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


namespace Leggiero
{
	// Foraward Declaration
	class IGame;


	namespace Platform
	{
		namespace iOS
		{
			// Forward Declarations
            class IOSTextInputDialog;
            class IOSWebView;


			// Platform Application
			class IOSPlatformApplication
				: public IPlatformApplication
				, public Application::ApplicationComponent
				, public Application::IAppInformationProvider, public Application::IDeviceInformationProvider
				, public Application::IPlatformAppControl
				, public Application::SystemEventDispatcher, public Application::RawTouchEventDispatcher
			{
			public:
                IOSPlatformApplication();
				virtual ~IOSPlatformApplication();

			public:
				// Process Game Lifetime Events
				void InitializeGame();
				void TerminateGame();

				void InitializeGameGraphic();
				void TerminateGameGraphic();

				void DrawFrame();

				// Set Application Meta-Data
				void SetScreenSize(int width, int height, bool isFirstSetting);
				void SetSafePadding(int left, int top, int right, int bottom);
				void SetApproximateScreenPPI(float approxXPPI, float approxYPPI);
				void SetIsTablet(bool isTablet);
				void SetSystemLocale(const std::string &systemLocaleString);

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
				virtual Application::DeviceScreenCoordType GetPixelWidth() const override { return m_screenWidth; }
				virtual Application::DeviceScreenCoordType GetPixelHeight() const override { return m_screenHeight; }
				virtual Application::DeviceScreenPixelCoordType GetIntegerPixelWidth() const override { return m_screenPixelWidth; }
				virtual Application::DeviceScreenPixelCoordType GetIntegerPixelHeight() const override { return m_screenPixelHeight; }

				virtual Application::AppPlatformType GetPlatformType() const override { return Application::AppPlatformType::kAndroid; }

				virtual const char *GetSystemLocaleString() const override { return m_systemLocaleString.c_str(); }

				virtual std::string GetAppReferrer() override;

			public:	// IDeviceInformationProvider
				virtual Application::DeviceScreenCoordType GetScreenPixelWidth() const override { return m_screenWidth; }
				virtual Application::DeviceScreenCoordType GetScreenPixelHeight() const override { return m_screenHeight; }
				virtual Application::DeviceScreenPixelCoordType GetScreenIntegerPixelWidth() const override { return m_screenPixelWidth; }
				virtual Application::DeviceScreenPixelCoordType GetScreenIntegerPixelHeight() const override { return m_screenPixelHeight; }

				virtual Application::DeviceScreenCoordType GetApproximateScreenXPPI() const override { return m_approxScreenXPPI; }
				virtual Application::DeviceScreenCoordType GetApproximateScreenYPPI() const override { return m_approxScreenYPPI; }

				virtual Utility::Math::BasicRect<Application::DeviceScreenCoordType> GetSafeArea() const override { return m_safeAreaRect; }

				virtual GameDataString GetDeviceName() const override;
				virtual bool IsTablet() const override { return m_isTablet; }
				
			public:	// IPlatformAppControl
				// Set whether to prevent device sleep on idle
				virtual void SetNoSleepMode(bool isPreventSleep) override;

			public:	// Process System Events
				void OnPause();
				void OnResume();

				void OnGoToBackground();
				void OnReturnFromBackground();

				void OnMemoryWarning();

				void OnBackButtonPressed();

			public:	// Process Touch Events
				void OnTouchDown(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType xPos, Application::DeviceScreenCoordType yPos, GameTimeClockType::time_point eventTime);
				void OnTouchMove(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType xPos, Application::DeviceScreenCoordType yPos, GameTimeClockType::time_point eventTime);
				void OnTouchUp(Application::DeviceTouchIdType touchId, Application::DeviceScreenCoordType xPos, Application::DeviceScreenCoordType yPos, GameTimeClockType::time_point eventTime);
                void OnTouchCancel(Application::DeviceTouchIdType touchId, GameTimeClockType::time_point eventTime);

				void CancelAllTouch();

			protected:
				std::shared_ptr<IGame> m_game;

				Application::DeviceScreenPixelCoordType m_screenPixelWidth;
				Application::DeviceScreenPixelCoordType m_screenPixelHeight;
				Application::DeviceScreenCoordType		m_screenWidth;
				Application::DeviceScreenCoordType		m_screenHeight;

				Application::DeviceScreenCoordType		m_approxScreenXPPI;
				Application::DeviceScreenCoordType		m_approxScreenYPPI;

				Utility::Math::BasicRect<Application::DeviceScreenCoordType> m_safeAreaRect;

				bool			m_isTablet;
				std::string		m_systemLocaleString;

				std::shared_ptr<IOSTextInputDialog> m_textInputDialogController;
				std::shared_ptr<IOSWebView> m_webViewController;

				std::unordered_set<Application::DeviceTouchIdType> m_downedTouches;
			};
		}
	}
}

#endif
