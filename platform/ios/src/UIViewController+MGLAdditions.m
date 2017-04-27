#import "UIViewController+MGLAdditions.h"

@implementation UIViewController (MGLAdditions)

+ (UIViewController *)getTopViewControllerFrom:(UIViewController *)viewController
{
    if ([viewController isKindOfClass:[UINavigationController class]])
    {
        return [UIViewController getTopViewControllerFrom:[((UINavigationController *)viewController) visibleViewController]];
    }
    else if ([viewController isKindOfClass:[UITabBarController class]])
    {
        return [UIViewController getTopViewControllerFrom:[((UITabBarController *)viewController) selectedViewController]];
    }
    else if (viewController.presentedViewController)
    {
        return [UIViewController getTopViewControllerFrom:viewController.presentedViewController];
    }
    return viewController;
}

@end
