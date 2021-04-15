////////////////////////////////////////////////////////////////////////////////
// ${{ProgramName}}iOSSetting.h (${{ProgramName}} - iOS)
//
// iOS Application Setting
////////////////////////////////////////////////////////////////////////////////

#ifndef __${{ProgramName-Upper}}_APP_IOS__${{ProgramName-Upper}}_IOS_SETTING_H
#define __${{ProgramName-Upper}}_APP_IOS__${{ProgramName-Upper}}_IOS_SETTING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Application
#include <Engine/Application/IGameLaunchOption.h>


namespace ${{ProgramName}}
{
    // Code Level Settings
    namespace Setting
    {
        extern bool kIsLandscapeMode;
    }


	// iOS Application Launch Setting
	class ${{ProgramName}}iOSSetting
		: public Leggiero::IGameLaunchOption
	{
	public:
        ${{ProgramName}}iOSSetting();
		virtual ~${{ProgramName}}iOSSetting();
	};
}

#endif
