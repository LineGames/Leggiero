////////////////////////////////////////////////////////////////////////////////
// LeggieroAppDelegate.mm (Leggiero - Platform.iOS)
//
// AppDelegate Base Class Implementations
////////////////////////////////////////////////////////////////////////////////

#import "LeggieroAppDelegate.h"

// Leggiero.Platform.iOS
#include "iOSPlatformApplication.h"
#include "LeggieroIOSCommon.h"


@interface LeggieroAppDelegate ()

@end


@implementation LeggieroAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Override point for customization after application launch.
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application
{
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        pApp->OnPause();
    }
}


- (void)applicationDidEnterBackground:(UIApplication *)application
{
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        pApp->OnGoToBackground();
    }
}


- (void)applicationWillEnterForeground:(UIApplication *)application
{
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        pApp->OnReturnFromBackground();
    }
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        pApp->OnResume();
    }
}


- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    Leggiero::Platform::iOS::IOSPlatformApplication *pApp = Leggiero::Platform::iOS::pApplication;
    if (pApp != nullptr)
    {
        pApp->OnMemoryWarning();
    }
}


- (BOOL)application:(UIApplication *)app
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options
{
    return NO;
}

@end
