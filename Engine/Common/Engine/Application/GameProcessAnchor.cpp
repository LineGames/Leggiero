////////////////////////////////////////////////////////////////////////////////
// Application/GameProcessAnchor.cpp (Leggiero - Engine)
//
// Implementation for Game Process Anchor Class
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GameProcessAnchor.h"

// Leggiero.Engine
#include "GameProcessAnchorObserver.h"


namespace Leggiero
{
	namespace Engine
	{
		//////////////////////////////////////////////////////////////////////////////// GameProcessAnchor

		//------------------------------------------------------------------------------
		GameProcessAnchor::GameProcessAnchor()
		{
		}

		//------------------------------------------------------------------------------
		GameProcessAnchor::~GameProcessAnchor()
		{
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::RegisterOnPrepare(GameProcessAnchorObserver::IPrepareHandler *handler)
		{
			m_onPrepareNotifier.RegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::UnRegisterOnPrepare(GameProcessAnchorObserver::IPrepareHandler *handler)
		{
			m_onPrepareNotifier.UnRegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::RegisterOnGraphicPrepare(GameProcessAnchorObserver::IGraphicPrepareHandler *handler)
		{
			m_onGraphicPrepareNotifier.RegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::UnRegisterOnGraphicPrepare(GameProcessAnchorObserver::IGraphicPrepareHandler *handler)
		{
			m_onGraphicPrepareNotifier.UnRegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::RegisterOnBeforeFrame(GameProcessAnchorObserver::IBeforeFrameHandler *handler)
		{
			m_onBeforeFrameNotifier.RegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::UnRegisterOnBeforeFrame(GameProcessAnchorObserver::IBeforeFrameHandler *handler)
		{
			m_onBeforeFrameNotifier.UnRegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::RegisterOnAfterFrame(GameProcessAnchorObserver::IAfterFrameHandler *handler)
		{
			m_onAfterFrameNotifier.RegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::UnRegisterOnAfterFrame(GameProcessAnchorObserver::IAfterFrameHandler *handler)
		{
			m_onAfterFrameNotifier.UnRegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::RegisterOnGraphicShutdown(GameProcessAnchorObserver::IGraphicShutdownHandler *handler)
		{
			m_onGraphicShutdownNotifier.RegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::UnRegisterOnGraphicShutdown(GameProcessAnchorObserver::IGraphicShutdownHandler *handler)
		{
			m_onGraphicShutdownNotifier.UnRegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::RegisterOnShutdown(GameProcessAnchorObserver::IShutdownHandler *handler)
		{
			m_onShutdownNotifier.RegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::UnRegisterOnShutdown(GameProcessAnchorObserver::IShutdownHandler *handler)
		{
			m_onShutdownNotifier.UnRegisterObserver(handler);
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::NotifyPrepare()
		{
			m_onPrepareNotifier.NotifyEvent([](GameProcessAnchorObserver::IPrepareHandler *handler) { handler->GameProcess_OnPrepare(); });
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::NotifyGraphicPrepare(bool isFirstPrepare)
		{
			m_onGraphicPrepareNotifier.NotifyEvent([isFirstPrepare](GameProcessAnchorObserver::IGraphicPrepareHandler *handler) { handler->GameProcess_OnGraphicPrepare(isFirstPrepare); });
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::NotifyBeforeFrame(GameFrameNumberType frameNumber, GameTimeClockType::time_point frameTime)
		{
			m_onBeforeFrameNotifier.NotifyEvent([frameNumber, frameTime](GameProcessAnchorObserver::IBeforeFrameHandler *handler) { handler->GameProcess_OnBeforeFrame(frameNumber, frameTime); });
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::NotifyAfterFrame(GameFrameNumberType frameNumber)
		{
			m_onAfterFrameNotifier.NotifyEvent([frameNumber](GameProcessAnchorObserver::IAfterFrameHandler *handler) { handler->GameProcess_OnAfterFrame(frameNumber); });
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::NotifyGraphicShutdown()
		{
			m_onGraphicShutdownNotifier.NotifyEvent([](GameProcessAnchorObserver::IGraphicShutdownHandler *handler) { handler->GameProcess_OnGraphicShutdown(); });
		}

		//------------------------------------------------------------------------------
		void GameProcessAnchor::NotifyShutdown()
		{
			m_onShutdownNotifier.NotifyEvent([](GameProcessAnchorObserver::IShutdownHandler *handler) { handler->GameProcess_OnShutdown(); });
		}
	}
}
