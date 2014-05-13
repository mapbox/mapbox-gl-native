//
//  MBXAppDelegate.m
//  ios
//
//  Created by Justin R. Miller on 1/27/14.
//
//

#import "MBXAppDelegate.h"

#import "MBXViewController.h"

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [MBXViewController new];
    [self.window makeKeyAndVisible];

    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [(MBXViewController *)self.window.rootViewController saveState];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    [(MBXViewController *)self.window.rootViewController restoreState];
}


@end