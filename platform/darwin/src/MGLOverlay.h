#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLAnnotation.h"
#import "MGLGeometry.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLOverlay` protocol defines a specific type of annotation that represents
 both a point and an area on a map. Overlay objects are essentially data objects
 that contain the geographic data needed to represent the map area. For example,
 overlays can take the form of common shapes such as rectangles and circles.
 They can also describe polygons and other complex shapes.
 
 You use overlays to layer more sophisticated content on top of a map view. For
 example, you could use an overlay to show the boundaries of a national park or
 trace a bus route along city streets. This SDK defines several concrete classes
 that conform to this protocol and define standard shapes.
 
 Because overlays are also annotations, they have similar usage pattern to
 annotations. When added to a map view using the `-addOverlay:` method, that
 view detects whenever the overlay’s defined region intersects the visible
 portion of the map. At that point, the map view asks its delegate to provide a
 special overlay view to draw the visual representation of the overlay. If you
 add an overlay to a map view as an annotation instead, it is treated as an
 annotation with a single point.
 */
@protocol MGLOverlay <MGLAnnotation>

/**
 The approximate center point of the overlay area. (required) (read-only)
 
 This point is typically set to the center point of the map’s bounding
 rectangle. It is used as the anchor point for any callouts displayed for the
 annotation.
 */
@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;

/**
 The cooordinate rectangle that encompasses the overlay. (required) (read-only)
 
 This property contains the smallest rectangle that completely encompasses the
 overlay. Implementers of this protocol must set this area when implementing
 their overlay class, and after setting it, you must not change it.
 */
@property (nonatomic, readonly) MGLCoordinateBounds overlayBounds;

/**
 Returns a Boolean indicating whether the specified rectangle intersects the
 receiver’s shape.
 
 You can implement this method to provide more specific bounds checking for an
 overlay. If you do not implement it, the bounding rectangle is used to detect
 intersections.
 
 @param overlayBounds The rectangle to intersect with the receiver’s area.
 @return `YES` if any part of the map rectangle intersects the receiver’s shape
    or `NO` if it does not.
 */
- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds;

@end

NS_ASSUME_NONNULL_END
