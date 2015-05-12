#import "MGLTAppDelegate.h"
#import "MGLTViewController.h"
#import "MapboxGL.h"

@implementation MGLTAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    UINavigationController *wrapper = [[UINavigationController alloc] initWithRootViewController:[MGLTViewController new]];
    self.window.rootViewController = wrapper;
    wrapper.navigationBarHidden = YES;
    wrapper.toolbarHidden = YES;
    [self.window makeKeyAndVisible];

    [MGLAccountManager setMapboxMetricsEnabledSettingShownInApp:YES]; // a lie, but a convenient one

    return YES;
}

@end
