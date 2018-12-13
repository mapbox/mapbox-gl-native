#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLScaleBar : UIView

// Sets the scale and redraws the scale bar
@property (nonatomic, assign) CLLocationDistance metersPerPoint;

/**
 A `CGSize` indicating intrinsic content size of the scale bar.
 */
- (CGSize)intrinsicContentSize;

@end
