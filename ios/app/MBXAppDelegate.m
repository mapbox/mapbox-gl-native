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

/**
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Example of how to resume Metrics Collection
    
    // Reasons for needing to resume:
    // 1. In UIBackground and app starts listening for Location Updates where it previously had not been listening.
    // 2. App is entering foreground where it had called pauseMetricsCollection.
    [MGLMapboxEvents resumeMetricsCollection];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Example of how to pause Metrics Collection
    
    // Reason for needing to pause:
    // 1. Either entering or already in UIBackground and app stops listening for Location Updates
    // via any CLLocationManager instance it may have.
    [MGLMapboxEvents pauseMetricsCollection];
}
*/

@end
