#import "MBXAppDelegate.h"
#import "MBXViewController.h"
#import <mbgl/ios/MGLMapboxEvents.h>

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [[UINavigationController alloc] initWithRootViewController:[MBXViewController new]];
    [self.window makeKeyAndVisible];

    return YES;
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    [MGLMapboxEvents resumeMetricsCollection];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [MGLMapboxEvents pauseMetricsCollection];
}


@end
