#import "MBXAppDelegate.h"
#import "MBXViewController.h"

@implementation MBXAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [[UINavigationController alloc] initWithRootViewController:[MBXViewController new]];
    [self.window makeKeyAndVisible];

    return YES;
}

@end
