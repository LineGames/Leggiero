////////////////////////////////////////////////////////////////////////////////
// iOSPlatformUtility.mm (Leggiero - Platform.iOS)
//
// iOS Platform Utility Functions
////////////////////////////////////////////////////////////////////////////////

// My Header
#import "iOSPlatformUtility.h"

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// System Library
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

// External Library
#include <sys/utsname.h>


namespace Leggiero
{
    namespace Platform
    {
        namespace iOS
        {
            namespace _Internal
            {
                bool g_isAppLandscapeMode = false;
            }
        
            //------------------------------------------------------------------------------
            void SetIsLandscapeMode(bool isLandscapeMode)
            {
                _Internal::g_isAppLandscapeMode = isLandscapeMode;
            }
        
            //------------------------------------------------------------------------------
            bool IsAppLandscapeMode()
            {
                return _Internal::g_isAppLandscapeMode;
            }
        
            //------------------------------------------------------------------------------
            bool IsIPadMini()
            {
                struct utsname systemInfo;
                if (uname(&systemInfo) < 0)
                {
                    return false;
                }
                
                id machineName = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
                
                if ([machineName isEqualToString:@"iPad2,5"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad2,6"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad2,7"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad4,4"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad4,5"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad4,6"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad4,7"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad4,8"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad4,9"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad5,1"])
                {
                    return true;
                }
                if ([machineName isEqualToString:@"iPad5,2"])
                {
                    return true;
                }
                
                return false;
            }
            
            //------------------------------------------------------------------------------
            float CalculatePPI(float screenWidth, float screenHeight)
            {
                constexpr float kResolutionEpsilon = 0.001f;
                auto _IsSameFloatFunc = [kResolutionEpsilon] (float a, float b)
                {
                    float diff = a - b;
                    return ((diff < kResolutionEpsilon) && (diff > -kResolutionEpsilon));
                };
                
                constexpr float kIPhoneXWidth = 2436.0f;
                constexpr float kIPhoneXHeight = 1125.0f;
                constexpr float kIPhoneXPPI = 458.0f;
                
                constexpr float kIPhone6Width = 1334.0f;
                constexpr float kIPhone6Height = 750.0f;
                constexpr float kIPhone6PPI = 326.0f;
                
                constexpr float kIPhone6PWidth = 1920.0f;
                constexpr float kIPhone6PHeight = 1080.0f;
                constexpr float kIPhone6PPPI = 401.0f;
                
                constexpr float kIPhone5Width = 1136.0f;
                constexpr float kIPhone5Height = 640.0f;
                constexpr float kIPhone5PPI = 326.0f;
                
                constexpr float kIPhone4Width = 960.0f;
                constexpr float kIPhone4Height = 640.0f;
                constexpr float kIPhone4PPI = 326.0f;
                
                constexpr float kIPhoneOldWidth = 480.0f;
                constexpr float kIPhoneOldHeight = 320.0f;
                constexpr float kIPhoneOldPPI = 163.0f;
                
                constexpr float kIPadRetinaWidth = 2048.0f;
                constexpr float kIPadRetinaHeight = 1536.0f;
                constexpr float kIPadRetinaPPI = 264.0f;
                constexpr float kIPadMiniRetinaPPI = 326.0f;
                
                constexpr float kIPadOldWidth = 1024.0f;
                constexpr float kIPadOldHeight = 768.0f;
                constexpr float kIPadOldPPI = 132.0f;
                constexpr float kIPadMiniOldPPI = 163.0f;
                
                constexpr float kIPadProWidth = 2732.0f;
                constexpr float kIPadProHeight = 2048.0f;
                constexpr float kIPadPro10Width = 2732.0f;
                constexpr float kIPadPro10Height = 2048.0f;
                constexpr float kIPadProPPI = 264.0f;
                
                // Checking in Landscape Mode
                float width = screenWidth;
                float height = screenHeight;
                
                if (_IsSameFloatFunc(width, kIPhone6Width) && _IsSameFloatFunc(height, kIPhone6Height))
                {
                    // iPhone6, 7, 8 (S)
                    return kIPhone6PPI;
                }
                if (_IsSameFloatFunc(width, kIPhone6PWidth) && _IsSameFloatFunc(height, kIPhone6PHeight))
                {
                    // iPhone6, 7, 8 Plus (S)
                    return kIPhone6PPPI;
                }
                if (_IsSameFloatFunc(width, kIPhone5Width) && _IsSameFloatFunc(height, kIPhone5Height))
                {
                    // iPhone5 Series / iPhone SE / iPod Touch 5th and 6th generation
                    return kIPhone5PPI;
                }
                if (_IsSameFloatFunc(width, kIPhone4Width) && _IsSameFloatFunc(height, kIPhone4Height))
                {
                    // iPhone4 Series / iPod Touch 4th generation
                    return kIPhone4PPI;
                }
                if (_IsSameFloatFunc(width, kIPhoneOldWidth) && _IsSameFloatFunc(height, kIPhoneOldHeight))
                {
                    // iPhone3 Series and Older / iPod Touch 3rd and older generations
                    return kIPhoneOldPPI;
                }
                if (_IsSameFloatFunc(width, kIPhoneXWidth) && _IsSameFloatFunc(height, kIPhoneXHeight))
                {
                    // iPhoneX
                    return kIPhoneXPPI;
                }
                
                if (_IsSameFloatFunc(width, kIPadRetinaWidth) && _IsSameFloatFunc(height, kIPadRetinaHeight))
                {
                    if (IsIPadMini())
                    {
                        // iPad Mini 2nd and later generations
                        return kIPadMiniRetinaPPI;
                    }
                    else
                    {
                        // iPad 3rd and later generations / iPad Pro 9.7 inch
                        return kIPadRetinaPPI;
                    }
                }
                if (_IsSameFloatFunc(width, kIPadOldWidth) && _IsSameFloatFunc(height, kIPadOldHeight))
                {
                    if (IsIPadMini())
                    {
                        // iPad Mini 1st generation
                        return kIPadMiniOldPPI;
                    }
                    else
                    {
                        // iPad 2nd and older generations
                        return kIPadOldPPI;
                    }
                }
                
                if (_IsSameFloatFunc(width, kIPadProWidth) && _IsSameFloatFunc(height, kIPadProHeight))
                {
                    // iPad Pro 12.9 inch
                    return kIPadProPPI;
                }
                if (_IsSameFloatFunc(width, kIPadPro10Width) && _IsSameFloatFunc(height, kIPadPro10Height))
                {
                    // iPad Pro 10.5 inch
                    return kIPadProPPI;
                }
                
                // Default Calculation
                float calcScale = 1.0f;
                if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
                {
                    calcScale = [[UIScreen mainScreen] scale];
                }
                if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
                {
                    return 132.0f * calcScale;
                }
                if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
                {
                    return 163.0f * calcScale;
                }
                
                return 160.0f * calcScale;
            }
        }
    }
}
