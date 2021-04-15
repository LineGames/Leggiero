////////////////////////////////////////////////////////////////////////////////
// GraphicsModuleInterface.cpp (Leggiero/Modules - Graphics)
//
// Common Implementation related to Graphics Module
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GraphicsModuleInterface.h"

// Leggiero.Engine
#include <Engine/Application/IPlatformApplication.h>
#include <Engine/Application/GameProcessAnchor.h>
#include <Engine/Application/GameProcessAnchorObserver.h>

// Leggiero.Graphics
#include "GraphicsThreadContext.h"
#include "_Internal/_InternalInitializer.h"
#include "_Internal/_InternalUpdater.h"


LEGGIERO_DEFINE_SINGLETON_MEMBERS(Leggiero::Graphics::GraphicsModule);
DEFINE_GET_SINGLETON_MODULE_INTERFACE(Leggiero::Graphics::GraphicsModule, Leggiero::EngineModuleIdType::kGraphics);


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Module Internal Implementations

		namespace _Internal
		{
			// Game Process Event Anchor for Graphics Module
			class GraphicsGameProcessAnchor
				: public Engine::GameProcessAnchorObserver::IGraphicPrepareHandler
				, public Engine::GameProcessAnchorObserver::IBeforeFrameHandler
			{
			public:	// Observer Interface
				virtual void GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare) override
				{
					_Internal::_InitializeReferenceStateInGraphicThread();
					InitializeMainGLContextInGraphicThread();
					ReferenceState::_Internal::_UpdateReferenceStateAtFrame();
				}

				virtual void GameProcess_OnBeforeFrame(GameFrameNumberType frameNumber, GameTimeClockType::time_point frameReferenceTime) override
				{
					ReferenceState::_Internal::_UpdateReferenceStateAtFrame();
				}
			};

			//------------------------------------------------------------------------------
			GraphicsGameProcessAnchor g_anchorObject;
		}


		//////////////////////////////////////////////////////////////////////////////// GraphicsModule

		//------------------------------------------------------------------------------
		void GraphicsModule::_InitializeCurrentModule(IPlatformApplication *app, Engine::GameProcessAnchor *gameAnchor)
		{
			_Internal::_InitializeSharedInternalState();
			_Internal::_InitializeReferenceState();
			_Internal::_InitializeApplicationReferenceState(app->GetApplicationComponent());

			gameAnchor->RegisterOnGraphicPrepare(&_Internal::g_anchorObject);
			gameAnchor->RegisterOnBeforeFrame(&_Internal::g_anchorObject);
		}

		//------------------------------------------------------------------------------
		void GraphicsModule::_FinalizeCurrentModule()
		{
			_Internal::_FinalizeReferenceState();
			_Internal::_FinalizeSharedInternalState();
		}
	}
}
