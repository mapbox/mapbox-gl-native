#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLScaleBar : UIView

// Sets the scale and redraws the scale bar
@property (nonatomic, assign) CLLocationDistance metersPerPoint;

// Expose for testing
@property (nonatomic, readonly) NSArray<UIView *> *labelViews;
@property (nonatomic, readonly) NSArray<UIView *> *bars;
@property (nonatomic, readonly) UIView *containerView;
@property (nonatomic, readonly) CGSize size;
@property (nonatomic) NSNumber *testingRightToLeftOverride;
@end
