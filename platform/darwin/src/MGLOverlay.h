#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLAnnotation.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLOverlay` protocol defines a specific type of annotation that represents
 both a point and an area on a map. Overlay objects are essentially data objects
 that contain the geographic data needed to represent the map area. Overlays can
 take the form of a polyline or polygon.

 You use overlays to layer more sophisticated content on top of a map view. For
 example, you could use an overlay to show the boundaries of a national park or
 trace a bus route along city streets. This SDK defines several concrete classes
 that conform to this protocol and define standard shapes.
 */
@protocol MGLOverlay <MGLAnnotation>

/**
 A coordinate representing the overlay. (required) (read-only)
 */
@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;

/**
 The cooordinate rectangle that encompasses the overlay. (required) (read-only)

 This property contains the smallest rectangle that completely encompasses the
 overlay. Implementers of this protocol must set this area when implementing
 their overlay class, and after setting it, you must not change it.
 
 If this overlay spans the antimeridian, its bounds may extend west of −180 degrees
 longitude or east of 180 degrees longitude. For example, an overlay covering the
 Pacific Ocean from Tokyo to San Francisco might have a bounds extending
 from (35.68476, −220.24257) to (37.78428, −122.41310).
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
