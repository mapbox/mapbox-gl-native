#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLScaleBar : UIView

// Sets the scale and fades in the scale bar
@property (nonatomic, assign) CLLocationDistance metersPerPoint;

// Fades out the scale bar
- (void)fadeOut;

@end
