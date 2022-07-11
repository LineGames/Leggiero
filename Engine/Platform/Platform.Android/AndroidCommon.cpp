////////////////////////////////////////////////////////////////////////////////
// AndroidCommon.cpp (Leggiero - Platform.Android)
//
// Android Common Implementations
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "AndroidCommon.h"

// Standard Library
#include <cstdlib>

// Sytem Library
#include <sys/system_properties.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// Common Utilites

			//------------------------------------------------------------------------------
			int GetDeviceAndroidAPILevel()
            {
                char sdkVersionStringBuffer[PROP_VALUE_MAX];

                if (__system_property_get("ro.build.version.sdk", sdkVersionStringBuffer))
                {
                    int sdkVersion = atoi(sdkVersionStringBuffer);
                    return sdkVersion;
                }

                return -1;
            }
		}
	}
}
