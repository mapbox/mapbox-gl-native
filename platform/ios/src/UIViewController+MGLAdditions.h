#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIViewController (MGLAdditions)

+ (UIViewController *)mgl_topMostViewControllerInHierarchyWith:(UIViewController *)viewController;

@end

NS_ASSUME_NONNULL_END
