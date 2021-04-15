////////////////////////////////////////////////////////////////////////////////
// Application/GameProcessAnchorObserver.h (Leggiero - Engine)
//
// Observer Interfaces for Game Process Anchor
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENGINE__APPLICATION__GAME_PROCESS_ANCHOR_OBSERVER_H
#define __ENGINE__APPLICATION__GAME_PROCESS_ANCHOR_OBSERVER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Engine
	{
		namespace GameProcessAnchorObserver
		{
			// On Prepare Event Handler
			class IPrepareHandler
			{
			public:
				virtual void GameProcess_OnPrepare() = 0;
			};


			// On Graphic Prepare Event Handler
			class IGraphicPrepareHandler
			{
			public:
				virtual void GameProcess_OnGraphicPrepare(bool isFirstGraphicPrepare) = 0;
			};


			// On Before Frame Event Handler
			class IBeforeFrameHandler
			{
			public:
				virtual void GameProcess_OnBeforeFrame(GameFrameNumberType frameNumber, GameTimeClockType::time_point frameReferenceTime) = 0;
			};


			// On After Frame Event Handler
			class IAfterFrameHandler
			{
			public:
				virtual void GameProcess_OnAfterFrame(GameFrameNumberType frameNumber) = 0;
			};


			// On Graphic Shutdown Event Handler
			class IGraphicShutdownHandler
			{
			public:
				virtual void GameProcess_OnGraphicShutdown() = 0;
			};


			// On Shutdown Event Handler
			class IShutdownHandler
			{
			public:
				virtual void GameProcess_OnShutdown() = 0;
			};
		}
	}
}

#endif
