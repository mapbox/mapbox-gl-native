#import "MGLUserLocation.h"

@interface MGLUserLocation (Private)

@property (nonatomic, assign, readwrite) CLLocationCoordinate2D coordinate;
@property (nonatomic, strong, readwrite) CLLocation *location;
@property (nonatomic, strong, readwrite) CLHeading *heading;

@end
