////////////////////////////////////////////////////////////////////////////////
// Scene/UISceneBase.h (Leggiero/Modules - LegacyUI)
//
// Base class for UI using scene
// This is not mandatory to use UI in scene structure, but it can be a reference
// to using UI in a game scene.
// 
// This scene basically depends on following components:
//   * ApplicationComponent (Application)
//   * TouchInputComponent (Input)
//   * GraphicResourceManagerComponent (Graphics)
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__SCENE__UI_SCENE_BASE_H
#define __LM_LUI__SCENE__UI_SCENE_BASE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// External Library
#include <glm/glm.hpp>

// Leggiero.Engine
#include <Engine/Application/IGame.h>
#include <Engine/Toolbox/Scene/IGameScene.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Input
#include <Input/Touch/TouchEvent.h>


namespace Leggiero
{
	// Forward Declaration
	namespace Input
	{
		namespace Touch
		{
			class EventBasedTouchContext;
		}
	}


	namespace LUI
	{
		// Forward Declarations
		class UIManager;


		// UI Scene Base Class
		class UISceneBase
			: public Toolbox::Scene::IGameScene
			, public Application::IGraphicEventObserver
		{
		public:
			UISceneBase(Toolbox::Scene::IGameSceneContext *ownerContext, IGame *game);
			virtual ~UISceneBase();

		public:	// IGameScene
			// To override these functions, please call parent's function to correct behavior
			
			// Re-Initialize Scene's Graphic Contents (after scene's graphic contents invalidated)
			// Called in Graphic Thread
			virtual void ReInitializeSceneGraphic() override;

			// Prepare Scene Just Before Enter
			virtual void BeforeSceneEnter() override;

			// Process Scene Just After Exit
			virtual void AfterSceneExit() override;

			// Process a Game Frame
			virtual void ProcessFrame(GameTimeClockType::time_point frameReferenceTime) override;

		protected:	// IGameScene
			// To override these functions, please call parent's function to correct behavior

			// Initialize Scene Contents
			virtual void _InitializeScene() override;

			// Initialize Scene's Graphic Contents
			// Called in Graphic Thread
			virtual void _InitializeSceneGraphic() override;

			// Finalize Scene Contents
			virtual void _FinalizeScene() override;

			// Finalize Scene's Graphic Contents
			// Called in Graphic Thread
			virtual void _FinalizeSceneGraphic() override;

		protected:	// Overriding Interfaces
			// Clear Screen Background
			virtual void _ClearScreen();

			// Initialize UI Manager
			// Default implementation is create scene's own manager.
			// Override this to use another shared manager object, and so on...
			virtual void _InitializeUIManager();

			// Contents Interface
			// Called in Graphic Thread
			virtual void _InitializeUIContents() { }
			virtual void _FinalizeUIContents() { }

			virtual void _SetUIBeforeEnterFrame() { }
			virtual void _UnSetUIAfterExitFrame() { }

			// To override core logic without pain
			virtual void _ProcessFrameLogic(GameTimeClockType::time_point frameReferenceTime, GameTimeClockType::duration frameInterval) { }
			virtual void _BeforeFrameProcess() { }
			virtual void _AfterFrameProcess() { }

			virtual void _ProcessAfterUIUpdate() { }

			virtual void _RenderPreUIContents() { }
			virtual void _RenderPostUIContents() { }

		public:	// Application::IGraphicEventObserver
			virtual void OnScreenSizeChanged(Application::DeviceScreenCoordType width, Application::DeviceScreenCoordType height) override;
			virtual void OnSafeAreaConfigurationChanged() override;

		protected:
			virtual void _PrepareScreenTransform(Application::DeviceScreenCoordType width = static_cast<Application::DeviceScreenCoordType>(-1.0f), Application::DeviceScreenCoordType height = static_cast<Application::DeviceScreenCoordType>(-1.0f));

		protected:
			IGame *m_game;

			GameTimeClockType::time_point	m_lastFrameTime;
			GameTimeClockType::duration		m_recentFrameInterval;

			std::shared_ptr<UIManager>		m_uiManager;

			std::shared_ptr<Input::Touch::EventBasedTouchContext>	m_uiTouchContext;
			std::vector<Input::Touch::TouchEvent>					m_uiTouchEventQueueBuffer;

			glm::mat4 m_screenUIProjectionMatrix;
			glm::mat4 m_screenUIViewMatrix;
		};
	}
}

#endif
