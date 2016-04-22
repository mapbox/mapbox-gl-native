#import <UIKit/UIKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationView : UIView

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier;

/**
 The string that identifies that this annotation view is reusable. (read-only)
 */
@property (nonatomic, readonly, nullable) NSString *reuseIdentifier;

- (void)prepareForReuse;

@end

NS_ASSUME_NONNULL_END
