////////////////////////////////////////////////////////////////////////////////
// RawTouchEventObserver.h (Leggiero/Modules - Application)
//
// Definition of Raw Touch Event from Platform Observer Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__RAW_TOUCH_EVENT_OBSERVER_H
#define __LM_APPLICATION__RAW_TOUCH_EVENT_OBSERVER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Application
#include "DeviceCommonTypes.h"


namespace Leggiero
{
	namespace Application
	{
		// Observer Interface to receive Platform Raw Touch Event
		class IRawTouchInputEventObserver
		{
		public:	// Basic Touch Primitives
			virtual void OnTouchDown(DeviceTouchIdType touchId, DeviceScreenCoordType x, DeviceScreenCoordType y, GameTimeClockType::time_point eventTime) { }
			virtual void OnTouchMoved(DeviceTouchIdType touchId, DeviceScreenCoordType x, DeviceScreenCoordType y, GameTimeClockType::time_point eventTime) { }
			virtual void OnTouchUp(DeviceTouchIdType touchId, DeviceScreenCoordType x, DeviceScreenCoordType y, GameTimeClockType::time_point eventTime) { }
			virtual void OnTouchCanceled(DeviceTouchIdType touchId, GameTimeClockType::time_point eventTime) { }
		};
	}
}

#endif
