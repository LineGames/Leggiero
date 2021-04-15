////////////////////////////////////////////////////////////////////////////////
// SystemEventObserver.h (Leggiero/Modules - Application)
//
// Definition of System Event Observer Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__SYSTEM_EVENT_OBSERVER_H
#define __LM_APPLICATION__SYSTEM_EVENT_OBSERVER_H


// Leggiero.Application
#include "DeviceCommonTypes.h"


namespace Leggiero
{
	namespace Application
	{
		//////////////////////////////////////////////////////////////////////////////// IApplicationEventObserver

		// Observer Interface to receive Application Event
		class IApplicationEventObserver
		{
		public:
			// Pause
			virtual void OnPause() { }
			virtual void OnResume() { }

			// Background
			virtual void OnGoToBackground() { }
			virtual void OnReturnFromBackground() { }
		};


		//////////////////////////////////////////////////////////////////////////////// ISystemCommandEventObserver

		// Observer Interface to receive Platform System Command Event
		class ISystemCommandEventObserver
		{
		public:
			// Back-Button
			// return means back button handled by
			virtual bool OnBackButtonPressed() { return false; }
		};


		//////////////////////////////////////////////////////////////////////////////// ISystemEventObserver

		// Observer Interface to receive Platform System Event
		class ISystemEventObserver
		{
		public:
			// Memory Warning
			virtual void OnMemoryWarning() { }
		};


		//////////////////////////////////////////////////////////////////////////////// IGraphicEventObserver

		// Observer Interface to receive Graphic Resource Event
		class IGraphicEventObserver
		{
		public:
			virtual void OnScreenSizeChanged(DeviceScreenCoordType width, DeviceScreenCoordType height) { }
			virtual void OnSafeAreaConfigurationChanged() { }

			virtual void OnGraphicInitialized() { }
		};
	}
}

#endif
