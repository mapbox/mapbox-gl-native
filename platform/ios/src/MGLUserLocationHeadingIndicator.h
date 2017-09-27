#import <QuartzCore/QuartzCore.h>
#import "MGLUserLocationAnnotationView.h"

@protocol MGLUserLocationHeadingIndicator <NSObject>

- (instancetype)initWithUserLocationAnnotationView:(MGLUserLocationAnnotationView *)userLocationView;
- (void)updateHeadingAccuracy:(CLLocationDirection)accuracy;
- (void)updateTintColor:(CGColorRef)color;

@end
