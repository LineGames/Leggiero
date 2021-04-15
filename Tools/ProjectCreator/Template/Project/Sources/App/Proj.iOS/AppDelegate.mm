////////////////////////////////////////////////////////////////////////////////
// AppDelegate.mm (${{ProgramName}} - iOS)
//
// Game App Delegate Implementation
////////////////////////////////////////////////////////////////////////////////

#import "AppDelegate.h"

#include "${{ProgramName}}iOSSetting.h"


@interface AppDelegate ()

@end


@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [super application:application didFinishLaunchingWithOptions:launchOptions];
    return YES;
}


- (BOOL)application:(UIApplication *)app
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options {
    [super application:app openURL:url options:options];
    return NO;
}


-(UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
    if (${{ProgramName}}::Setting::kIsLandscapeMode)
    {
        return UIInterfaceOrientationMaskLandscape;
    }
    else
    {
        return UIInterfaceOrientationMaskPortrait;
    }
}

@end
