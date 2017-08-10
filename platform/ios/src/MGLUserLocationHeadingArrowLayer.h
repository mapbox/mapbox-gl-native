#import <QuartzCore/QuartzCore.h>
#import "MGLUserLocationAnnotationView.h"

@interface MGLUserLocationHeadingArrowLayer : CAShapeLayer

- (instancetype)initWithUserLocationAnnotationView:(MGLUserLocationAnnotationView *)userLocationView;
- (void)updateTintColor:(CGColorRef)color;

@end
