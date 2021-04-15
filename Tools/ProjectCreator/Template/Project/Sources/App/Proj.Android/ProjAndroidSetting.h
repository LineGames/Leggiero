////////////////////////////////////////////////////////////////////////////////
// ${{ProgramName}}AndroidSetting.h (${{ProgramName}} - ${{ProgramName}}.Android)
//
// Android Application Setting
////////////////////////////////////////////////////////////////////////////////

#ifndef __${{ProgramName-Upper}}_APP_ANDROID__${{ProgramName-Upper}}_ANDROID_SETTING_H
#define __${{ProgramName-Upper}}_APP_ANDROID__${{ProgramName-Upper}}_ANDROID_SETTING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Application
#include <Engine/Application/IGameLaunchOption.h>


namespace ${{ProgramName}}
{
	// Android Application Launch Setting
	class ${{ProgramName}}AndroidSetting
		: public Leggiero::IGameLaunchOption
	{
	public:
		${{ProgramName}}AndroidSetting();
		virtual ~${{ProgramName}}AndroidSetting();
	};
}

#endif
