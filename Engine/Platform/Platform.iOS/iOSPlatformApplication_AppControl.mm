////////////////////////////////////////////////////////////////////////////////
// iOSPlatformApplication_AppControl.mm (Leggiero - Platform.iOS)
//
// Implementation of iOS Platform Application's App Control Parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSPlatformApplication.h"

// System Library
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			//////////////////////////////////////////////////////////////////////////////// IOSPlatformApplication - Platform Application Controls

			//------------------------------------------------------------------------------
			// Set whether to prevent device sleep on idle
			void IOSPlatformApplication::SetNoSleepMode(bool isPreventSleep)
			{
				if (isPreventSleep)
				{
					[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
				}
				else
				{
					[[UIApplication sharedApplication] setIdleTimerDisabled:NO];
				}
			}
		}
	}
}
