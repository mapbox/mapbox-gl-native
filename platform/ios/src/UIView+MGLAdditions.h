#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView (MGLAdditions)

- (nullable UIViewController *)mgl_viewControllerForLayoutGuides;

- (NSLayoutYAxisAnchor *)mgl_safeTopAnchor;

- (NSLayoutXAxisAnchor *)mgl_safeLeadingAnchor;

- (NSLayoutYAxisAnchor *)mgl_safeBottomAnchor;

- (NSLayoutXAxisAnchor *)mgl_safeTrailingAnchor;

- (CGRect)mgl_frameForIdentifyTransform;

@end

NS_ASSUME_NONNULL_END
