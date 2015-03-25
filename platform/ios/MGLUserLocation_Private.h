#import "MGLUserLocation.h"

@interface MGLUserLocation (Private)

@property (nonatomic, readwrite) CLLocationCoordinate2D coordinate;
@property (nonatomic, readwrite) CLLocation *location;
@property (nonatomic, readwrite) CLHeading *heading;

@end
