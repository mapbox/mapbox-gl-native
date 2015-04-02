#import <Foundation/Foundation.h>

/// The degree to which the map view tracks the user’s location.
typedef NS_ENUM(NSUInteger, MGLUserTrackingMode)
{
    MGLUserTrackingModeNone              = 0,   ///< does not track the user’s location or heading
    MGLUserTrackingModeFollow            = 1,   ///< tracks the user’s location
    MGLUserTrackingModeFollowWithHeading = 2,   ///< tracks the user’s location and heading
};
