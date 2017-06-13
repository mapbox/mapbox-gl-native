//
//  AppDelegate.m
//  Examples
//
//  Created by Jason Wray on 1/26/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end
@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Speed-up Core Animation-based animations in testing scenarios.
    if ([[NSProcessInfo processInfo].arguments containsObject:@"useFastAnimations"]) {
        self.window.layer.speed = 100;
    }

    return YES;
}

@end
