#import <Mapbox/Mapbox.h>

namespace mbgl {
    class Map;
    class Renderer;
}

/// Minimum size of an annotation’s accessibility element.
extern const CGSize MGLAnnotationAccessibilityElementMinimumSize;

@interface MGLMapView (Private)

/// Currently shown popover representing the selected annotation.
@property (nonatomic) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;

/** Triggers another render pass even when it is not necessary. */
- (void)setNeedsGLDisplay;

- (mbgl::Map *)mbglMap;

- (mbgl::Renderer *)renderer;

/** Returns whether the map view is currently loading or processing any assets required to render the map */
- (BOOL)isFullyLoaded;

/** Empties the in-memory tile cache. */
- (void)didReceiveMemoryWarning;

@end
