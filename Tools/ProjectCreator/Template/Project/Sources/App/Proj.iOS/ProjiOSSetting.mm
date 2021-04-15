////////////////////////////////////////////////////////////////////////////////
// ${{ProgramName}}iOSSetting.mm (${{ProgramName}} - iOS)
//
// Basic Implementation for iOS Application Setting
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "${{ProgramName}}iOSSetting.h"

// Leggiero.Platform.iOS
#include <iOSPlatformSetting.h>


namespace ${{ProgramName}}
{
    //////////////////////////////////////////////////////////////////////////////// Code Level Settings

    //------------------------------------------------------------------------------
    namespace Setting
    {
        bool kIsLandscapeMode = ${{Orientation-IsLandscapeCppBool}};
    }


	//////////////////////////////////////////////////////////////////////////////// ${{ProgramName}}iOSSetting

	//------------------------------------------------------------------------------
    ${{ProgramName}}iOSSetting::${{ProgramName}}iOSSetting()
	{
	}

	//------------------------------------------------------------------------------
    ${{ProgramName}}iOSSetting::~${{ProgramName}}iOSSetting()
	{

	}
}


//////////////////////////////////////////////////////////////////////////////// iOS Platform

namespace Leggiero
{
    namespace Platform
    {
        namespace iOS
        {
            //------------------------------------------------------------------------------
            IGameLaunchOption *CreateGameLaunchOption()
            {
                // For now, setting contains nothing
                return new ${{ProgramName}}::${{ProgramName}}iOSSetting();
            }
        }
    }
}
