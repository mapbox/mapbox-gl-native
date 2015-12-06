#import <mbgl/osx/MGLMapView.h>

void mgl_linkMapViewIBCategory();

@interface MGLMapView (Private)

/// True if the view or application is in a state where it is not expected to be
/// actively drawing.
@property (nonatomic, readonly, getter=isDormant) BOOL dormant;

/// Center latitude set independently of the center longitude in an inspectable.
@property (nonatomic) CLLocationDegrees pendingLatitude;
/// Center longitude set independently of the center latitude in an inspectable.
@property (nonatomic) CLLocationDegrees pendingLongitude;

/// Synchronously render a frame of the map.
- (void)renderSync;

@end
