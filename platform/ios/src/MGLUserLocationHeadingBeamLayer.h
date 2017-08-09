#import <QuartzCore/QuartzCore.h>
#import "MGLUserLocationAnnotationView.h"

@interface MGLUserLocationHeadingBeamLayer : CALayer

- (instancetype)initWithUserLocationAnnotationView:(MGLUserLocationAnnotationView *)userLocationView;
- (void)updateHeadingAccuracy:(CLLocationDirection)accuracy;
- (void)updateTintColor:(CGColorRef)color;

@end
