#import "MGLMapView.h"
#import "MGLUserLocationAnnotationView.h"
#import "MGLAnnotationContainerView.h"

#include <mbgl/util/size.hpp>

namespace mbgl {
    class Map;
    class Renderer;
}

class MGLMapViewImpl;
@class MGLSource;

/// Standard animation duration for UI elements.
FOUNDATION_EXTERN const NSTimeInterval MGLAnimationDuration;

/// Minimum size of an annotation’s accessibility element.
FOUNDATION_EXTERN const CGSize MGLAnnotationAccessibilityElementMinimumSize;

/// Indicates that a method (that uses `mbgl::Map`) was called after app termination.
FOUNDATION_EXTERN MGL_EXPORT MGLExceptionName const _Nonnull MGLUnderlyingMapUnavailableException;

@interface MGLMapView (Private)

/// The map view’s OpenGL rendering context.
@property (nonatomic, readonly, nullable) EAGLContext *context;

/// Currently shown popover representing the selected annotation.
@property (nonatomic, nonnull) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;

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
- (void)didFailToLoadImage:(nonnull NSString *)imageName;
- (BOOL)shouldRemoveStyleImage:(nonnull NSString *)imageName;

/** Triggers another render pass even when it is not necessary. */
- (void)setNeedsRerender;

/// Synchronously render a frame of the map.
- (void)renderSync;

- (nonnull mbgl::Map *)mbglMap;

- (nonnull mbgl::Renderer *)renderer;

/** Returns whether the map view is currently loading or processing any assets required to render the map */
- (BOOL)isFullyLoaded;

/** Empties the in-memory tile cache. */
- (void)didReceiveMemoryWarning;

/** Returns an instance of MGLMapView implementation. Used for integration testing. */
- (nonnull MGLMapViewImpl *) viewImpl;

- (void)pauseRendering:(nonnull NSNotification *)notification;
- (void)resumeRendering:(nonnull NSNotification *)notification;
@property (nonatomic, nonnull) MGLUserLocationAnnotationView *userLocationAnnotationView;
@property (nonatomic, nonnull) MGLAnnotationContainerView *annotationContainerView;
@property (nonatomic, readonly) BOOL enablePresentsWithTransaction;

- (BOOL) _opaque;

@end
