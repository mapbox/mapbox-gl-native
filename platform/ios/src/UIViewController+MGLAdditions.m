#import "UIViewController+MGLAdditions.h"

@implementation UIViewController (MGLAdditions)

+ (UIViewController *)mgl_topMostViewControllerInHierarchyWith:(UIViewController *)viewController
{
    if ([viewController isKindOfClass:[UINavigationController class]])
    {
        return [UIViewController mgl_topMostViewControllerInHierarchyWith:[((UINavigationController *)viewController) visibleViewController]];
    }
    else if ([viewController isKindOfClass:[UITabBarController class]])
    {
        return [UIViewController mgl_topMostViewControllerInHierarchyWith:[((UITabBarController *)viewController) selectedViewController]];
    }
    else if (viewController.presentedViewController)
    {
        return [UIViewController mgl_topMostViewControllerInHierarchyWith:viewController.presentedViewController];
    }
    return viewController;
}

@end
