#import <QuartzCore/QuartzCore.h>
#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocationHeadingIndicator.h"

@interface MGLUserLocationHeadingArrowLayer : CAShapeLayer <MGLUserLocationHeadingIndicator>

- (instancetype)initWithUserLocationAnnotationView:(MGLUserLocationAnnotationView *)userLocationView;
- (void)updateHeadingAccuracy:(CLLocationDirection)accuracy;
- (void)updateTintColor:(CGColorRef)color;

@end
