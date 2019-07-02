#import "MGLMapView.h"

#include <mbgl/util/size.hpp>

namespace mbgl {
    class Map;
    class Renderer;
}

@class MGLSource;

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

/// The map view’s OpenGL rendering context, if it is backed by an OpenGL based view.
@property (readonly, nonatomic, nullable) CGLContextObj context;

- (mbgl::Size)framebufferSize;

/// Map observers
- (void)cameraWillChangeAnimated:(BOOL)animated;
- (void)cameraIsChanging;
- (void)cameraDidChangeAnimated:(BOOL)animated;
- (void)mapViewWillStartLoadingMap;
- (void)mapViewDidFinishLoadingMap;
- (void)mapViewDidFailLoadingMapWithError:(nonnull NSError *)error;
- (void)mapViewWillStartRenderingFrame;
- (void)mapViewDidFinishRenderingFrameFullyRendered:(BOOL)fullyRendered;
- (void)mapViewWillStartRenderingMap;
- (void)mapViewDidFinishRenderingMapFullyRendered:(BOOL)fullyRendered;
- (void)mapViewDidBecomeIdle;
- (void)mapViewDidFinishLoadingStyle;
- (void)sourceDidChange:(nonnull MGLSource *)source;
- (BOOL)shouldRemoveStyleImage:(nonnull NSString *)imageName;

/// Asynchronously render a frame of the map.
- (void)setNeedsRerender;

/// Synchronously render a frame of the map.
- (void)renderSync;

- (BOOL)isTargetingInterfaceBuilder;

- (nonnull mbgl::Map *)mbglMap;

- (nonnull mbgl::Renderer *)renderer;

@end
