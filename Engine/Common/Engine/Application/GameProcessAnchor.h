////////////////////////////////////////////////////////////////////////////////
// Application/GameProcessAnchor.h (Leggiero - Engine)
//
// Anchor Center for Game Process Events
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__GAME_PROCESS_ANCHOR_H
#define __ENGINE__APPLICATION__GAME_PROCESS_ANCHOR_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>

// Leggiero.Utility
#include <Utility/Sugar/EventNotifier.h>


namespace Leggiero
{
	namespace Engine
	{
		// Forward Declaration for Observers
		namespace GameProcessAnchorObserver
		{
			class IPrepareHandler;
			class IGraphicPrepareHandler;
			class IBeforeFrameHandler;
			class IAfterFrameHandler;
			class IGraphicShutdownHandler;
			class IShutdownHandler;
		}


		// Game Process Events Anchor
		class GameProcessAnchor
		{
		public:
			GameProcessAnchor();
			virtual ~GameProcessAnchor();

		public:	// Anchor Functions
			void RegisterOnPrepare(GameProcessAnchorObserver::IPrepareHandler *handler);
			void UnRegisterOnPrepare(GameProcessAnchorObserver::IPrepareHandler *handler);

			void RegisterOnGraphicPrepare(GameProcessAnchorObserver::IGraphicPrepareHandler *handler);
			void UnRegisterOnGraphicPrepare(GameProcessAnchorObserver::IGraphicPrepareHandler *handler);

			void RegisterOnBeforeFrame(GameProcessAnchorObserver::IBeforeFrameHandler *handler);
			void UnRegisterOnBeforeFrame(GameProcessAnchorObserver::IBeforeFrameHandler *handler);

			void RegisterOnAfterFrame(GameProcessAnchorObserver::IAfterFrameHandler *handler);
			void UnRegisterOnAfterFrame(GameProcessAnchorObserver::IAfterFrameHandler *handler);

			void RegisterOnGraphicShutdown(GameProcessAnchorObserver::IGraphicShutdownHandler *handler);
			void UnRegisterOnGraphicShutdown(GameProcessAnchorObserver::IGraphicShutdownHandler *handler);

			void RegisterOnShutdown(GameProcessAnchorObserver::IShutdownHandler *handler);
			void UnRegisterOnShutdown(GameProcessAnchorObserver::IShutdownHandler *handler);

		public:	// Notify Functions Called by Game
			void NotifyPrepare();
			void NotifyGraphicPrepare(bool isFirstPrepare);
			void NotifyBeforeFrame(GameFrameNumberType frameNumber, GameTimeClockType::time_point frameTime);
			void NotifyAfterFrame(GameFrameNumberType frameNumber);
			void NotifyGraphicShutdown();
			void NotifyShutdown();

		protected:	// Real Notifiers
			Utility::DesignPattern::EventNotifier<GameProcessAnchorObserver::IPrepareHandler *>			m_onPrepareNotifier;
			Utility::DesignPattern::EventNotifier<GameProcessAnchorObserver::IGraphicPrepareHandler *>	m_onGraphicPrepareNotifier;
			Utility::DesignPattern::EventNotifier<GameProcessAnchorObserver::IBeforeFrameHandler *>		m_onBeforeFrameNotifier;
			Utility::DesignPattern::EventNotifier<GameProcessAnchorObserver::IAfterFrameHandler *>		m_onAfterFrameNotifier;
			Utility::DesignPattern::EventNotifier<GameProcessAnchorObserver::IGraphicShutdownHandler *>	m_onGraphicShutdownNotifier;
			Utility::DesignPattern::EventNotifier<GameProcessAnchorObserver::IShutdownHandler *>		m_onShutdownNotifier;
		};
	}
}

#endif
