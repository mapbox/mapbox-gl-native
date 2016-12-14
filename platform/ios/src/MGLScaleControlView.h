#import <UIKit/UIKit.h>

@interface MGLScaleControlView : UIView

- (void)updateWithZoomLevel:(double)zoomLevel metersPerPoint:(CLLocationDistance)metersPerPoint;

@end
