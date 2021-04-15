////////////////////////////////////////////////////////////////////////////////
// Touch/TouchEvent.h (Leggiero/Modules - Input)
//
// Each Touch Event
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_INPUT__TOUCH__TOUCH_EVENT_H
#define __LM_INPUT__TOUCH__TOUCH_EVENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <limits>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.Input
#include "../InputCommonTypes.h"


namespace Leggiero
{
	namespace Input
	{
		namespace Touch
		{
			// Touch Event Type
			enum class TouchEventType
			{
				kINVALID = 0x0,

				kTouchStateChanged = 0x1,
				kTouchUpdatePosition = 0x2,
				kTouchIsForcedChange = 0x4,

				kTouchStarted = 0x10,
				kTouchEnded = 0x20,

				kDown = (kTouchStateChanged | kTouchUpdatePosition | kTouchStarted),
				kMove = (kTouchUpdatePosition),
				kUp = (kTouchStateChanged | kTouchUpdatePosition | kTouchEnded),
				kCancel = (kTouchStateChanged | kTouchIsForcedChange | kTouchEnded),
			};

			MAKE_ENUM_FLAG(TouchEventType);

			constexpr bool IsValidTouchEvent(TouchEventType v) { return (v != TouchEventType::kINVALID); }


			// Touch Event Data Structure
			struct TouchEvent
			{
			public:
				TouchEventType	eventType;

				TouchIdType		touchId;
				TouchCoordType	x;
				TouchCoordType	y;
				GameTimeClockType::time_point eventTime;

			public:
				TouchEvent()
					: eventType(TouchEventType::kINVALID)
					, x(std::numeric_limits<TouchCoordType>::quiet_NaN())
					, y(std::numeric_limits<TouchCoordType>::quiet_NaN())
				{ }

				TouchEvent(TouchEventType eventType, TouchIdType touchId, GameTimeClockType::time_point eventTime,
					TouchCoordType x = std::numeric_limits<TouchCoordType>::quiet_NaN(), TouchCoordType y = std::numeric_limits<TouchCoordType>::quiet_NaN())
					: eventType(eventType), touchId(touchId), eventTime(eventTime), x(x), y(y)
				{ }

				bool IsValid() const { return IsValidTouchEvent(eventType); }
				void MakeInvalid() { eventType = TouchEventType::kINVALID; }

				void ReInitialize(TouchEventType newEventType, TouchIdType newTouchId, GameTimeClockType::time_point newEventTime,
					TouchCoordType newX = std::numeric_limits<TouchCoordType>::quiet_NaN(), TouchCoordType newY = std::numeric_limits<TouchCoordType>::quiet_NaN())
				{
					eventType = newEventType;
					touchId = newTouchId;
					eventTime = newEventTime;
					x = newX;
					y = newY;
				}
			};
		}
	}
}

#endif
