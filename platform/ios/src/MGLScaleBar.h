#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLScaleBar : UIView

// Sets the scale and redraws the scale bar
@property (nonatomic, assign) CLLocationDistance metersPerPoint;

@end
