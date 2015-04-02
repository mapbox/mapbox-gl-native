#import <Foundation/Foundation.h>

/** The mode used to track the user location on the map. */
typedef NS_ENUM(NSUInteger, MGLUserTrackingMode)
{
    /** The map does not follow the user location. */
    MGLUserTrackingModeNone              = 0,
    /** The map follows the user location. */
    MGLUserTrackingModeFollow,
    /** The map follows the user location and rotates when the heading changes. */
    MGLUserTrackingModeFollowWithHeading
};
