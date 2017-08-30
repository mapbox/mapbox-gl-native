#import <QuartzCore/QuartzCore.h>
#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocationHeadingIndicator.h"

@interface MGLUserLocationHeadingBeamLayer : CALayer <MGLUserLocationHeadingIndicator>

- (MGLUserLocationHeadingBeamLayer *)initWithUserLocationAnnotationView:(MGLUserLocationAnnotationView *)userLocationView;
- (void)updateHeadingAccuracy:(CLLocationDirection)accuracy;
- (void)updateTintColor:(CGColorRef)color;

@end
