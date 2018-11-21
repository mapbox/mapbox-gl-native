#import <Mapbox/Mapbox.h>

namespace mbgl {
    class Map;
    class Renderer;
}

/// Minimum size of an annotation’s accessibility element.
FOUNDATION_EXTERN const CGSize MGLAnnotationAccessibilityElementMinimumSize;

/// Indicates that a method (that uses `mbgl::Map`) was called after app termination.
FOUNDATION_EXTERN MGL_EXPORT MGLExceptionName const MGLUnderlyingMapUnavailableException;

@interface MGLMapView (Private)

/// Currently shown popover representing the selected annotation.
@property (nonatomic) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;

/** Triggers another render pass even when it is not necessary. */
- (void)setNeedsGLDisplay;

- (mbgl::Renderer *)renderer;

/** Returns whether the map view is currently loading or processing any assets required to render the map */
- (BOOL)isFullyLoaded;

/** Empties the in-memory tile cache. */
- (void)didReceiveMemoryWarning;

@end
