#import <mbgl/osx/MGLMapView.h>

@interface MGLMapView (Private)

@property (nonatomic, readonly, getter=isDormant) BOOL dormant;

@property (nonatomic) CLLocationDegrees pendingLatitude;
@property (nonatomic) CLLocationDegrees pendingLongitude;

- (void)renderSync;

@end
