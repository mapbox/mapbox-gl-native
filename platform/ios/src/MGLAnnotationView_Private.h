#import "MGLAnnotationView.h"
#import "MGLAnnotation.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationView (Private)

@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@property (nonatomic, readwrite, nullable) id <MGLAnnotation> annotation;

- (void)setCenter:(CGPoint)center pitch:(CGFloat)pitch;

@end

NS_ASSUME_NONNULL_END
