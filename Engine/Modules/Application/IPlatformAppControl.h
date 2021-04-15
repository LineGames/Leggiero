////////////////////////////////////////////////////////////////////////////////
// IPlatformAppControl.h (Leggiero/Modules - Application)
//
// Interface for Platform Application Control Sub-Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__I_PLATFORM_APP_CONTROL_H
#define __LM_APPLICATION__I_PLATFORM_APP_CONTROL_H


// Standard Library
#include <string>

// Leggiero.Application
#include "AppPlatformType.h"
#include "DeviceCommonTypes.h"


namespace Leggiero
{
	namespace Application
	{
		// Platform Application Control Interface
		class IPlatformAppControl
		{
		public:
			// Set whether to back button(Android) is always handled as default or not.
			// Initial value is true.
			virtual void SetBackButtonDefaultHandling(bool isHandle) { }

			// Set whether to prevent device sleep on idle
			virtual void SetNoSleepMode(bool isPreventSleep) = 0;
		};
	}
}

#endif
