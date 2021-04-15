////////////////////////////////////////////////////////////////////////////////
// IImmediateTouchInputEventObserver.h (Leggiero/Modules - Input)
//
// Observer Interface for Touch Input in Immediate Mode
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_INPUT__I_IMMEDIATE_TOUCH_INPUT_EVENT_OBSERVER_H
#define __LM_INPUT__I_IMMEDIATE_TOUCH_INPUT_EVENT_OBSERVER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Leggiero.Input
#include "InputCommonTypes.h"


namespace Leggiero
{
	namespace Input
	{
		// Observer Interface to Listen Touch Input Event
		class IImmediateTouchInputEventObserver
		{
		public:
			// Handle Touch Down Event
			virtual void OnTouchDown(TouchIdType touchId,
				TouchCoordType x, TouchCoordType y,
				GameTimeClockType::time_point eventTime)
			{ }

			// Handle Touch Move Event
			virtual void OnTouchMoved(TouchIdType touchId,
				TouchCoordType x, TouchCoordType y,
				GameTimeClockType::time_point eventTime)
			{ }

			// Handle Touch Up Event
			virtual void OnTouchUp(TouchIdType touchId,
				TouchCoordType x, TouchCoordType y,
				GameTimeClockType::time_point eventTime)
			{ }

			// Handle Touch Cancel Event by App Interrupt, or ...
			virtual void OnTouchCanceled(TouchIdType touchId,
				GameTimeClockType::time_point eventTime)
			{ }
		};
	}
}

#endif
