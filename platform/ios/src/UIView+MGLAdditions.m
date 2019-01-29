#import "UIView+MGLAdditions.h"

@implementation UIView (MGLAdditions)

- (UIViewController *)mgl_viewControllerForLayoutGuides
{
    // Per -[UIResponder nextResponder] documentation, a UIView’s next responder
    // is its managing UIViewController if applicable, or otherwise its
    // superview. UIWindow’s next responder is UIApplication, which has no next
    // responder.
    UIResponder *laterResponder = self;
    while ([laterResponder isKindOfClass:[UIView class]])
    {
        laterResponder = laterResponder.nextResponder;
    }
    if ([laterResponder isKindOfClass:[UIViewController class]])
    {
        return (UIViewController *)laterResponder;
    }
    return nil;
}

- (NSLayoutYAxisAnchor *)mgl_safeTopAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.topAnchor;
    } else {
        UIViewController *viewController = self.mgl_viewControllerForLayoutGuides;
        BOOL useLayoutGuides = viewController.view && viewController.automaticallyAdjustsScrollViewInsets;
        if (useLayoutGuides) {
            return viewController.topLayoutGuide.bottomAnchor;
        }
        else {
            return self.topAnchor;
        }
    }
}

- (NSLayoutXAxisAnchor *)mgl_safeLeadingAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.leadingAnchor;
    } else {
        return self.leadingAnchor;
    }
}

- (NSLayoutYAxisAnchor *)mgl_safeBottomAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.bottomAnchor;
    } else {
        UIViewController *viewController = self.mgl_viewControllerForLayoutGuides;
        BOOL useLayoutGuides = viewController.view && viewController.automaticallyAdjustsScrollViewInsets;
        if (useLayoutGuides) {
            return viewController.bottomLayoutGuide.topAnchor;
        }
        else {
            return self.bottomAnchor;
        }
    }
}

- (NSLayoutXAxisAnchor *)mgl_safeTrailingAnchor {
    if (@available(iOS 11.0, *)) {
        return self.safeAreaLayoutGuide.trailingAnchor;
    } else {
        return self.trailingAnchor;
    }
}

@end
