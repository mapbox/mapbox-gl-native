#import "MBXAppDelegate.h"
#import "MBXViewController.h"
#import <Mapbox/Mapbox.h>

@interface MBXAppDelegate() <MGLMetricsManagerDelegate>

@end

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
#ifndef MGL_DISABLE_LOGGING
    [MGLLoggingConfiguration sharedConfiguration].loggingLevel = MGLLoggingLevelFault;
#endif

    [MGLMetricsManager sharedManager].delegate = self;

    // Temporary key created just for this hack
    [MGLAccountManager setAccessToken:@"pk.eyJ1IjoidG1wc2FudG9zIiwiYSI6ImNqeXY0MTJ5ejBrNHMzYm40eTBtaTc5YncifQ.jb6YkzCG4QpJon7mdc4giA"];

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

- (BOOL)metricsManager:(MGLMetricsManager *)metricsManager shouldHandleMetric:(MGLMetricType)metricType {
    return YES;
}

- (void)metricsManager:(MGLMetricsManager *)metricsManager didCollectMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    [[MGLMetricsManager sharedManager] pushMetric:metricType withAttributes:attributes];
}

@end
