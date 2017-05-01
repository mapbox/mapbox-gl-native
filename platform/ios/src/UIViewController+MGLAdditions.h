#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController (MGLAdditions)

@property (readonly) UIViewController *mgl_topMostViewController;

- (UIViewController *)mgl_topMostViewControllerInHierarchyWith:(UIViewController *)viewController;

@end

NS_ASSUME_NONNULL_END
