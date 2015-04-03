#import "MGLUserLocation.h"

@class MGLMapView;

@interface MGLUserLocation (Private)

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readwrite) CLLocation *location;
@property (nonatomic, readwrite) CLHeading *heading;

@end
