#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A specialized view that displays the current compass heading for its associated map.
 */
@interface MGLCompassButton : UIImageView

/**
 The visibility of the compass button.

 You can configure a compass button to be visible all the time or only when the compass heading changes.
 */
@property (nonatomic, assign) MGLOrnamentVisibility compassVisibility;

// TODO: Make these private
+ (instancetype)compassButton;
- (void)updateCompassWithDirection:(CLLocationDirection)direction;

@end

NS_ASSUME_NONNULL_END
