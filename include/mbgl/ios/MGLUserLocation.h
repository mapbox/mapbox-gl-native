#import "MGLAnnotation.h"

@interface MGLUserLocation : NSObject <MGLAnnotation>

@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;

/** The current location of the device. (read-only)
*
*   This property contains `nil` if the map view is not currently showing the user location or if the user’s location has not yet been determined. */
@property (nonatomic, readonly) CLLocation *location;

/** The heading of the user location. (read-only)
*
* This property is `nil` if the user location tracking mode is not `RMUserTrackingModeFollowWithHeading`. */
@property (nonatomic, readonly) CLHeading *heading;

@end

