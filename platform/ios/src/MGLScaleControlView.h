#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLScaleControlView : UIView

- (void)updateWithZoomLevel:(double)zoomLevel metersPerPoint:(CLLocationDistance)metersPerPoint;

@end
