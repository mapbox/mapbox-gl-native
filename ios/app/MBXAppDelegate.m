#import "MBXAppDelegate.h"
#import "MBXViewController.h"
#import <mbgl/ios/MapboxGL.h>
#import <mbgl/ios/MGLAccountManager.h>
#import <mbgl/ios/MGLMapboxEvents.h>

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Set access token, unless MGLAccountManager already read it in from Info.plist.
    if ( ! [MGLAccountManager accessToken]) {
        NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
        if (accessToken) {
            // Store to preferences so that we can launch the app later on without having to specify
            // token.
            [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:@"access_token"];
        } else {
            // Try to retrieve from preferences, maybe we've stored them there previously and can reuse
            // the token.
            accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:@"access_token"];
        }
        if ( ! accessToken) NSLog(@"No access token set. Mapbox vector tiles won't work.");

        [MGLAccountManager setAccessToken:accessToken];
    }

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
