////////////////////////////////////////////////////////////////////////////////
// iOSPlatformApplication.mm (Leggiero - Platform.iOS)
//
// Implementation of iOS Platform Application
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSPlatformApplication.h"

// Leggiero.Engine
#include <Engine/Application/CreateGame.h>
#include <Engine/Application/IGame.h>
#include <Engine/Application/IGameLaunchOption.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Platform.iOS
#include "iOSPlatformSetting.h"
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
            IOSPlatformApplication::IOSPlatformApplication()
			{
				// Sub Components
				m_textInputDialogController = std::make_shared<IOSTextInputDialog>();
				m_webViewController = std::make_shared<IOSWebView>();
			}

			//------------------------------------------------------------------------------
            IOSPlatformApplication::~IOSPlatformApplication()
			{
				TerminateGame();
			}


			//////////////////////////////////////////////////////////////////////////////// IOSPlatformApplication - Game Lifetime

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::InitializeGame()
			{
				IGameLaunchOption *launchOption = CreateGameLaunchOption();
				m_game = CreateGame(this, launchOption);
				if (launchOption != nullptr)
				{
					delete launchOption;
				}

				m_game->Prepare();
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::TerminateGame()
			{
				if (!m_game)
				{
					return;
				}

				m_game->Shutdown();
				m_game.reset();
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::InitializeGameGraphic()
			{
				m_graphicEventNotifier.NotifyEvent(
					[](Application::IGraphicEventObserver *observer) mutable
					{
						observer->OnGraphicInitialized();
					});

				if (!m_game)
				{
					return;
				}
				m_game->GraphicPrepare();
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::TerminateGameGraphic()
			{
				if (!m_game)
				{
					return;
				}
				m_game->GraphicShutdown();
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::DrawFrame()
			{
				if (!m_game)
				{
					return;
				}
				m_game->UpdateFrame();
			}


			//////////////////////////////////////////////////////////////////////////////// IOSPlatformApplication - Metadata

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::SetScreenSize(int width, int height, bool isFirstSetting)
			{
				int oldPixelWidth = static_cast<int>(width);
				int oldPixelHeight = static_cast<int>(height);

				m_screenWidth = static_cast<Application::DeviceScreenCoordType>(width);
				m_screenHeight = static_cast<Application::DeviceScreenCoordType>(height);
				m_screenPixelWidth = static_cast<Application::DeviceScreenPixelCoordType>(width);
				m_screenPixelHeight = static_cast<Application::DeviceScreenPixelCoordType>(height);

				if (!isFirstSetting)
				{
					bool isChanged = ((oldPixelWidth != width) || (oldPixelHeight != height));
					if (isChanged)
					{
						// Update Safe Area
						m_safeAreaRect.right = static_cast<Application::DeviceScreenCoordType>(width) - static_cast<Application::DeviceScreenCoordType>(oldPixelWidth - m_safeAreaRect.right);
						m_safeAreaRect.bottom = static_cast<Application::DeviceScreenCoordType>(height) - static_cast<Application::DeviceScreenCoordType>(oldPixelHeight - m_safeAreaRect.bottom);

						// Notify Change
						Application::DeviceScreenPixelCoordType changedWidth = m_screenPixelWidth;
						Application::DeviceScreenPixelCoordType changedHeight = m_screenPixelHeight;
						m_graphicEventNotifier.NotifyEvent(
							[changedWidth, changedHeight](Application::IGraphicEventObserver *observer) mutable
							{
								observer->OnScreenSizeChanged(changedWidth, changedHeight);
								observer->OnSafeAreaConfigurationChanged();
							});
					}
				}
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::SetSafePadding(int left, int top, int right, int bottom)
			{
                Utility::Math::BasicRect<Application::DeviceScreenCoordType> newSafePadding(
                    static_cast<Application::DeviceScreenCoordType>(left),
                    static_cast<Application::DeviceScreenCoordType>(top),
                    m_screenWidth - static_cast<Application::DeviceScreenCoordType>(right),
                    m_screenHeight - static_cast<Application::DeviceScreenCoordType>(bottom)
                );
                if (newSafePadding.left != m_safeAreaRect.left || newSafePadding.top != m_safeAreaRect.top || newSafePadding.right != m_safeAreaRect.right || newSafePadding.bottom != m_safeAreaRect.bottom)
                {
                    m_safeAreaRect = newSafePadding;
                    m_graphicEventNotifier.NotifyEvent(
                        [](Application::IGraphicEventObserver *observer) mutable
                        {
                            observer->OnSafeAreaConfigurationChanged();
                        });
                }
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::SetApproximateScreenPPI(float approxXPPI, float approxYPPI)
			{
				m_approxScreenXPPI = static_cast<Application::DeviceScreenCoordType>(approxXPPI);
				m_approxScreenYPPI = static_cast<Application::DeviceScreenCoordType>(approxYPPI);
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::SetIsTablet(bool isTablet)
			{
				m_isTablet = isTablet;
			}

			//------------------------------------------------------------------------------
			void IOSPlatformApplication::SetSystemLocale(const std::string &systemLocaleString)
			{
				m_systemLocaleString = systemLocaleString;
			}
		}
	}
}
