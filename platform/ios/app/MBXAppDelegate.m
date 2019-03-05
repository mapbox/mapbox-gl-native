#import "MBXAppDelegate.h"
#import "MBXViewController.h"
#import <Mapbox/Mapbox.h>

NSString * const MBXMapboxAccessTokenDefaultsKey = @"MBXMapboxAccessToken";

@interface MBXAppDelegate() <MGLMetricsDelegate>

@end

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Set access token, unless MGLAccountManager already read it in from Info.plist.
    if ( ! [MGLAccountManager accessToken]) {
        NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
        if (accessToken) {
            // Store to preferences so that we can launch the app later on without having to specify
            // token.
            [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:MBXMapboxAccessTokenDefaultsKey];
        } else {
            // Try to retrieve from preferences, maybe we've stored them there previously and can reuse
            // the token.
            accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:MBXMapboxAccessTokenDefaultsKey];
        }
        [MGLAccountManager setAccessToken:accessToken];
#ifndef MGL_DISABLE_LOGGING
        [MGLLoggingConfiguration sharedConfiguration].loggingLevel = MGLLoggingLevelFault;
#endif
        [MGLMetricsManager sharedManager].delegate = self;
    }

    return YES;
}

#pragma mark - Quick actions

- (void)application:(UIApplication *)application performActionForShortcutItem:(UIApplicationShortcutItem *)shortcutItem completionHandler:(void (^)(BOOL))completionHandler {
    completionHandler([self handleShortcut:shortcutItem]);
}

- (BOOL)handleShortcut:(UIApplicationShortcutItem *)shortcut {
    if ([[shortcut.type componentsSeparatedByString:@"."].lastObject isEqual:@"settings"]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
        });

        return YES;
    }

    return NO;
}

- (BOOL)shouldHandleMetric:(MGLMetricType)metricType {
    return YES;
}

- (void)didCollectMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    [[MGLMetricsManager sharedManager] pushMetric:metricType withAttributes:attributes];
}

@end
