////////////////////////////////////////////////////////////////////////////////
// ${{ProgramName}}AndroidSetting.cpp (${{ProgramName}} - ${{ProgramName}}.Android)
//
// Basic Implementation for Android Application Setting
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "${{ProgramName}}AndroidSetting.h"

// Leggiero.Platform.Android
#include <AndroidPlatformSetting.h>


namespace ${{ProgramName}}
{
	//////////////////////////////////////////////////////////////////////////////// ${{ProgramName}}AndroidSetting

	//------------------------------------------------------------------------------
	${{ProgramName}}AndroidSetting::${{ProgramName}}AndroidSetting()
	{
	}

	//------------------------------------------------------------------------------
	${{ProgramName}}AndroidSetting::~${{ProgramName}}AndroidSetting()
	{

	}
}


//////////////////////////////////////////////////////////////////////////////// Android Platform

namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//------------------------------------------------------------------------------
			IGameLaunchOption *CreateGameLaunchOption()
			{
				// For now, setting contains nothing
				return new ${{ProgramName}}::${{ProgramName}}AndroidSetting();
			}
		}
	}
}
