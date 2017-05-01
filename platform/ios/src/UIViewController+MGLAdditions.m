#import "UIViewController+MGLAdditions.h"

@implementation UIViewController (MGLAdditions)

- (UIViewController *)mgl_topMostViewController
{
    return [self mgl_topMostViewControllerInHierarchyWith:self];
}

- (instancetype)mgl_topMostViewControllerInHierarchyWith:(UIViewController *)viewController
{
    if ([viewController isKindOfClass:[UINavigationController class]])
    {
        return [self mgl_topMostViewControllerInHierarchyWith:[(UINavigationController *)viewController visibleViewController]];
    }
    else if ([self isKindOfClass:[UITabBarController class]])
    {
        return [self mgl_topMostViewControllerInHierarchyWith:[((UITabBarController *)viewController) selectedViewController]];
    }
    else if (viewController.presentedViewController)
    {
        return [self mgl_topMostViewControllerInHierarchyWith:viewController.presentedViewController];
    }
    return viewController;
}

@end
