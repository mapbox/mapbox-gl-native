#import "MGLMapView.h"

namespace mbgl {
    class Map;
    class Renderer;
}

@interface MGLMapView (Private)

/// True if the view or application is in a state where it is not expected to be
/// actively drawing.
@property (nonatomic, readonly, getter=isDormant) BOOL dormant;

// These properties exist because initially, both the latitude and longitude are
// NaN. You have to set both the latitude and longitude simultaneously. If you
// set the latitude but reuse the current longitude, and the current longitude
// happens to be NaN, there will be no change because the resulting coordinate
// pair is invalid.

/// Center latitude set independently of the center longitude in an inspectable.
@property (nonatomic) CLLocationDegrees pendingLatitude;
/// Center longitude set independently of the center latitude in an inspectable.
@property (nonatomic) CLLocationDegrees pendingLongitude;

/// Asynchronously render a frame of the map.
- (void)setNeedsGLDisplay;

/// Synchronously render a frame of the map.
- (void)renderSync;

- (mbgl::Map *)mbglMap;

- (mbgl::Renderer *)renderer;

@end
