////////////////////////////////////////////////////////////////////////////////
// AndroidPlatformApplication.cpp (Leggiero - Platform.Android)
//
// Implementation of Android Platform Application
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidPlatformApplication.h"

// Leggiero.Engine
#include <Engine/Application/CreateGame.h>
#include <Engine/Application/IGame.h>
#include <Engine/Application/IGameLaunchOption.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Platform.Android
#include "AndroidPlatformSetting.h"
#include "Service/AndroidTextInputDialog.h"
#include "Service/AndroidWebView.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//------------------------------------------------------------------------------
			// Global App Instance
			AndroidPlatformApplication *AndroidPlatformApplication::s_globalAppInstance = nullptr;


			//////////////////////////////////////////////////////////////////////////////// AndroidPlatformApplication

			//------------------------------------------------------------------------------
			AndroidPlatformApplication::AndroidPlatformApplication()
				: m_isDefaultBackButtonHandling(true)
			{
				// Sub Components
				m_textInputDialogController = std::make_shared<AndroidTextInputDialog>();
				m_webViewController = std::make_shared<AndroidWebView>();
			}

			//------------------------------------------------------------------------------
			AndroidPlatformApplication::~AndroidPlatformApplication()
			{
				TerminateGame();
			}


			//////////////////////////////////////////////////////////////////////////////// AndroidPlatformApplication - Game Lifetime

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::InitializeGame()
			{
				if (m_game)
				{
					TerminateGame();
				}

				IGameLaunchOption *launchOption = CreateGameLaunchOption();
				m_game = CreateGame(this, launchOption);
				if (launchOption != nullptr)
				{
					delete launchOption;
				}

				m_game->Prepare();
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::TerminateGame()
			{
				if (!m_game)
				{
					return;
				}

				m_game->Shutdown();
				m_game.reset();
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::InitializeGameGraphic()
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
			void AndroidPlatformApplication::TerminateGameGraphic()
			{
				if (!m_game)
				{
					return;
				}
				m_game->GraphicShutdown();
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::DrawFrame()
			{
				if (!m_game)
				{
					return;
				}
				m_game->UpdateFrame();
			}


			//////////////////////////////////////////////////////////////////////////////// AndroidPlatformApplication - Metadata

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::SetScreenSize(int width, int height, bool isFirstSetting)
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
			void AndroidPlatformApplication::SetSafePadding(int left, int top, int right, int bottom, bool isFirstSetting)
			{
				if (isFirstSetting)
				{
					m_safeAreaRect.left = static_cast<Application::DeviceScreenCoordType>(left);
					m_safeAreaRect.top = static_cast<Application::DeviceScreenCoordType>(top);
					m_safeAreaRect.right = m_screenWidth - static_cast<Application::DeviceScreenCoordType>(right);
					m_safeAreaRect.bottom = m_screenHeight - static_cast<Application::DeviceScreenCoordType>(bottom);
				}
				else
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
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::SetApproximateScreenPPI(float approxXPPI, float approxYPPI)
			{
				m_approxScreenXPPI = static_cast<Application::DeviceScreenCoordType>(approxXPPI);
				m_approxScreenYPPI = static_cast<Application::DeviceScreenCoordType>(approxYPPI);
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::SetIsTablet(bool isTablet)
			{
				m_isTablet = isTablet;
			}

			//------------------------------------------------------------------------------
			void AndroidPlatformApplication::SetSystemLocale(const std::string &systemLocaleString)
			{
				m_systemLocaleString = systemLocaleString;
			}
		}
	}
}
