////////////////////////////////////////////////////////////////////////////////
// iOSPlatformUtility.h (Leggiero - Platform.iOS)
//
// iOS Platform Utility Functions
////////////////////////////////////////////////////////////////////////////////

#ifndef __PLATFORM__IOS_PLATFORM_UTILITY_H
#define __PLATFORM__IOS_PLATFORM_UTILITY_H


namespace Leggiero
{
    namespace Platform
    {
        namespace iOS
        {
            bool IsIPadMini();
            float CalculatePPI(float screenWidth, float screenHeight);
        
            void SetIsLandscapeMode(bool isLandscapeMode);
            bool IsAppLandscapeMode();
        
            namespace Graphics
            {
                void SetMainGLContextInformation(id mainContext);
            }
        }
    }
}

#endif
