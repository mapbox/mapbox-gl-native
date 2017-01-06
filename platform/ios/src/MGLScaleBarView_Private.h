#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLScaleBarView.h"

@interface MGLScaleBarView (Private)

@property (nonatomic, assign) CLLocationDistance metersPerPoint;

- (void)fadeOut;

@end
