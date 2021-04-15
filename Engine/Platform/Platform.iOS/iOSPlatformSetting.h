////////////////////////////////////////////////////////////////////////////////
// iOSPlatformSetting.h (Leggiero - Platform.iOS)
//
// iOS Platform Application Setting Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_IOS__IOS_PLATFORM_SETTING_H
#define __LP_IOS__IOS_PLATFORM_SETTING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	// Forward declaration of game launch option
	class IGameLaunchOption;


	namespace Platform
	{
		namespace iOS
		{
			IGameLaunchOption *CreateGameLaunchOption();
		}
	}
}

#endif
