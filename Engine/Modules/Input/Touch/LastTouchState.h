////////////////////////////////////////////////////////////////////////////////
// Touch/LastTouchState.h (Leggiero/Modules - Input)
//
// State of a touch
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_INPUT__TOUCH__LAST_TOUCH_STATE_H
#define __LM_INPUT__TOUCH__LAST_TOUCH_STATE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <limits>

// Leggiero.Input
#include "../InputCommonTypes.h"
#include "TouchEvent.h"


namespace Leggiero
{
	namespace Input
	{
		namespace Touch
		{
			// Touch State
			struct LastTouchState
			{
			public:
				TouchIdType	touchId;
				GameTimeClockType::time_point triggerEventTime;

				GameTimeClockType::time_point lastEventTime;

				TouchCoordType	x;
				TouchCoordType	y;

				bool isTouchDowned;

			public:
				LastTouchState()
					: touchId(std::numeric_limits<TouchIdType>::max()), x(std::numeric_limits<TouchCoordType>::quiet_NaN()), y(std::numeric_limits<TouchCoordType>::quiet_NaN()), isTouchDowned(false)
				{ }

				LastTouchState(const TouchEvent &triggerEvent)
					: isTouchDowned(true)
				{
					touchId = triggerEvent.touchId;
					triggerEventTime = triggerEvent.eventTime;
					lastEventTime = triggerEvent.eventTime;
					x = triggerEvent.x;
					y = triggerEvent.y;
				}

				// Update current touch state by event
				void UpdateTouchState(const TouchEvent &updatedEvent)
				{
					if (touchId != updatedEvent.touchId)
					{
						return;
					}
					if (updatedEvent.eventTime < lastEventTime)
					{
						return;
					}
					lastEventTime = updatedEvent.eventTime;
					if (Utility::SyntacticSugar::HasFlag(updatedEvent.eventType, TouchEventType::kTouchUpdatePosition))
					{
						x = updatedEvent.x;
						y = updatedEvent.y;
					}
					else
					{
						x = std::numeric_limits<TouchCoordType>::quiet_NaN();
						y = std::numeric_limits<TouchCoordType>::quiet_NaN();
					}
					if (Utility::SyntacticSugar::HasFlag(updatedEvent.eventType, TouchEventType::kTouchStateChanged))
					{
						if (Utility::SyntacticSugar::HasFlag(updatedEvent.eventType, TouchEventType::kTouchEnded))
						{
							isTouchDowned = false;
						}
						else if (Utility::SyntacticSugar::HasFlag(updatedEvent.eventType, TouchEventType::kTouchStarted))
						{
							isTouchDowned = true;
						}
					}
				}

				bool IsValid() const { return isfinite(x) && isfinite(y); }
			};
		}
	}
}

#endif
