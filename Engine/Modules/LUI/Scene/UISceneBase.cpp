////////////////////////////////////////////////////////////////////////////////
// Scene/UISceneBase.cpp (Leggiero/Modules - LegacyUI)
//
// Base implementation for UI using scene
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UISceneBase.h"

// Standard Library
#include <algorithm>

// External Library
#include <GLES3.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppInformationProvider.h>
#include <Application/SystemEventDispatcher.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>

// Leggiero.Input
#include <Input/TouchInputComponent.h>
#include <Input/Touch/EventBasedTouchContext.h>

// Leggiero.LegacyUI
#include "../UIManager.h"
#include "../Rendering/UIRenderer.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UISceneBase

		//------------------------------------------------------------------------------
		UISceneBase::UISceneBase(Toolbox::Scene::IGameSceneContext *ownerContext, IGame *game)
			: Toolbox::Scene::IGameScene(ownerContext), m_game(game)
		{
		}

		//------------------------------------------------------------------------------
		UISceneBase::~UISceneBase()
		{
		}

		//------------------------------------------------------------------------------
		// Re-Initialize Scene's Graphic Contents (after scene's graphic contents invalidated)
		// Called in Graphic Thread
		void UISceneBase::ReInitializeSceneGraphic()
		{
			if (m_uiManager)
			{
				m_uiManager->GraphicPrepare();
				m_uiManager->Renderer().SetColorModification();
			}
		}

		//------------------------------------------------------------------------------
		// Prepare Scene Just Before Enter
		void UISceneBase::BeforeSceneEnter()
		{
			m_lastFrameTime = GameTimeClockType::now();
			m_recentFrameInterval = GameTimeClockType::duration::zero();

			_PrepareScreenTransform();

			_SetUIBeforeEnterFrame();
			if (m_uiManager)
			{
				m_uiManager->Renderer().SetColorModification();
			}

			m_uiTouchContext->StartProcessing();

			m_game->GetComponent<Application::ApplicationComponent>()->SystemEventCenter().RegisterGraphicEventObserver(this);
		}

		//------------------------------------------------------------------------------
		// Process Scene Just After Exit
		void UISceneBase::AfterSceneExit()
		{
			m_game->GetComponent<Application::ApplicationComponent>()->SystemEventCenter().UnRegisterGraphicEventObserver(this);

			m_uiTouchContext->StopProcessing();
			m_uiTouchContext->ClearContextState();

			_UnSetUIAfterExitFrame();

			if (m_uiManager)
			{
				m_uiManager->Flush();
			}
		}

		//------------------------------------------------------------------------------
		// Process a Game Frame
		void UISceneBase::ProcessFrame(GameTimeClockType::time_point frameReferenceTime)
		{
			_BeforeFrameProcess();

			m_recentFrameInterval = frameReferenceTime - m_lastFrameTime;
			m_lastFrameTime = frameReferenceTime;

			// UI Inter-frame Logic
			if (m_uiManager)
			{
				// Process Touch
				while (true)
				{
					Input::Touch::TouchEvent dequeuedEvent = m_uiTouchContext->DequeueEvent();
					if (!dequeuedEvent.IsValid())
					{
						break;
					}
					m_uiTouchEventQueueBuffer.push_back(dequeuedEvent);
				}
				std::sort(m_uiTouchEventQueueBuffer.begin(), m_uiTouchEventQueueBuffer.end(), [](const Input::Touch::TouchEvent &a, Input::Touch::TouchEvent &b) {
					return (a.eventTime < b.eventTime);
					});

				m_uiManager->ProcessTouchEvents(m_uiTouchEventQueueBuffer, frameReferenceTime);
				m_uiTouchEventQueueBuffer.clear();
			}

			// Logic Process
			_ProcessFrameLogic(frameReferenceTime, m_recentFrameInterval);
			if (m_uiManager)
			{
				m_uiManager->UpdateFrame(m_recentFrameInterval);
				m_uiManager->UpdateLayout();
			}
			_ProcessAfterUIUpdate();

			// Render
			_ClearScreen();
			_RenderPreUIContents();
			if (m_uiManager)
			{
				m_uiManager->Renderer().SetScreenTransform(m_screenUIProjectionMatrix, m_screenUIViewMatrix);
				m_uiManager->Renderer().Render();
			}
			_RenderPostUIContents();

			_AfterFrameProcess();
		}

		//------------------------------------------------------------------------------
		// Initialize Scene Contents
		void UISceneBase::_InitializeScene()
		{
			m_uiTouchContext = std::make_shared<Input::Touch::EventBasedTouchContext>(m_game->GetComponent<Input::TouchInputComponent>());
		}

		//------------------------------------------------------------------------------
		// Initialize Scene's Graphic Contents
		// Called in Graphic Thread
		void UISceneBase::_InitializeSceneGraphic()
		{
			_InitializeUIManager();
			if (!m_uiManager)
			{
				// Something Wrong
				return;
			}

			_PrepareScreenTransform();
			m_uiManager->Renderer().SetColorModification();

			_InitializeUIContents();
			m_uiManager->UpdateLayout();
		}

		//------------------------------------------------------------------------------
		// Finalize Scene Contents
		void UISceneBase::_FinalizeScene()
		{
			m_uiTouchContext.reset();
		}

		//------------------------------------------------------------------------------
		// Finalize Scene's Graphic Contents
		// Called in Graphic Thread
		void UISceneBase::_FinalizeSceneGraphic()
		{
			_FinalizeUIContents();
			m_uiManager.reset();
		}

		//------------------------------------------------------------------------------
		void UISceneBase::_ClearScreen()
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		//------------------------------------------------------------------------------
		// Initialize UI Manager
		// Default implementation is create scene's own manager.
		void UISceneBase::_InitializeUIManager()
		{
			m_uiManager = std::make_shared<UIManager>();
			m_uiManager->Initialize(
				m_game->GetComponent<Application::ApplicationComponent>(),
				m_game->GetComponent<Graphics::GraphicResourceManagerComponent>()
			);
		}

		//------------------------------------------------------------------------------
		void UISceneBase::OnScreenSizeChanged(Application::DeviceScreenCoordType width, Application::DeviceScreenCoordType height)
		{
			_PrepareScreenTransform(width, height);
			if (m_uiManager)
			{
				m_uiManager->HintScreenDimensionChanged();
			}
		}

		//------------------------------------------------------------------------------
		void UISceneBase::OnSafeAreaConfigurationChanged()
		{
			if (m_uiManager)
			{
				m_uiManager->HintScreenDimensionChanged();
			}
		}

		//------------------------------------------------------------------------------
		void UISceneBase::_PrepareScreenTransform(Application::DeviceScreenCoordType width, Application::DeviceScreenCoordType height)
		{
			if (width < static_cast<Application::DeviceScreenCoordType>(0.0f) && height < static_cast<Application::DeviceScreenCoordType>(0.0f))
			{
				width = m_game->GetComponent<Application::ApplicationComponent>()->AppInformation().GetPixelWidth();
				height = m_game->GetComponent<Application::ApplicationComponent>()->AppInformation().GetPixelHeight();
			}

			m_screenUIProjectionMatrix = glm::ortho(0.0f, width, -height, 0.0f, -1.0f, 1.0f);

			glm::vec3 eyePos; eyePos[0] = 0.0f; eyePos[1] = 0.0f; eyePos[2] = -1.0f;
			glm::vec3 eyeSee; eyeSee[0] = eyePos[0]; eyeSee[1] = eyePos[1]; eyeSee[2] = 0.0f;
			glm::vec3 eyeUp; eyeUp[0] = 0.0f; eyeUp[1] = -1.0f; eyeUp[2] = 0.0f;
			m_screenUIViewMatrix = glm::lookAt(eyePos, eyeSee, eyeUp);
		}
	}
}
