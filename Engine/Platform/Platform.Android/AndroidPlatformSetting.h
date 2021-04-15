////////////////////////////////////////////////////////////////////////////////
// AndroidPlatformSetting.h (Leggiero - Platform.Android)
//
// Android Platform Application Setting Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_ANDROID__ANDROID_PLATFORM_SETTING_H
#define __LP_ANDROID__ANDROID_PLATFORM_SETTING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	// Forward declaration of game launch option
	class IGameLaunchOption;


	namespace Platform
	{
		namespace Android
		{
			IGameLaunchOption *CreateGameLaunchOption();
		}
	}
}

#endif
