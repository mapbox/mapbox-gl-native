#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLScaleBarView : UIView

@property (nonatomic, assign) CLLocationDistance metersPerPoint;

- (void)fadeOut;

@end
