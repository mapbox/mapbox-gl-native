#import "MBXAppDelegate.h"
#import "MBXViewController.h"

#import <HockeySDK/HockeySDK.h>

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [[UINavigationController alloc] initWithRootViewController:[MBXViewController new]];
    [self.window makeKeyAndVisible];

    // Mapbox devs: uncomment and insert Mapbox GL app id here
    //
    // [[BITHockeyManager sharedHockeyManager] configureWithIdentifier:@""];
    // [[BITHockeyManager sharedHockeyManager] startManager];
    // [[BITHockeyManager sharedHockeyManager].authenticator authenticateInstallation];

    return YES;
}

@end
