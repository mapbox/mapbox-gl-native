#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView (MGLAdditions)

- (NSLayoutYAxisAnchor *)safeTopAnchor;

- (NSLayoutXAxisAnchor *)safeLeadingAnchor;

- (NSLayoutYAxisAnchor *)safeBottomAnchor;

- (NSLayoutXAxisAnchor *)safeTrailingAnchor;

@end

NS_ASSUME_NONNULL_END
