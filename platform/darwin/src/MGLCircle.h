#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"
#import "MGLGeometry.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An `MGLCircle` object represents a closed, circular shape also known as a
 <a href="https://en.wikipedia.org/wiki/Spherical_cap">spherical cap</a>. A
 circle is defined by a center coordinate, specified as a
 `CLLocationCoordinate2D` instance, and a physical radius measured in meters.
 The circle is approximated as a polygon with a large number of vertices and
 edges. Due to the map’s Spherical Mercator projection, a large enough circle
 appears as an elliptical or even sinusoidal shape. You could use a circle to
 visualize for instance an impact zone, the `CLLocation.horizontalAccuracy` of a
 GPS location update, the regulated airspace around an airport, or the
 addressable consumer market around a city.

 You can add a circle overlay directly to a map view using the
 `-[MGLMapView addAnnotation:]` or `-[MGLMapView addOverlay:]` method. Configure
 a circle overlay’s appearance using
 `-[MGLMapViewDelegate mapView:strokeColorForShapeAnnotation:]` and
 `-[MGLMapViewDelegate mapView:fillColorForShape:]`.

 Alternatively, you can add a circle to the map by adding it to an
 `MGLShapeSource` object. Because GeoJSON cannot represent a curve per se, the
 circle is automatically converted to a polygon. See the `MGLPolygon` class for
 more information about polygons.

 Do not confuse this class with `MGLCircleStyleLayer`, which renders a circle
 defined by a center coordinate and a fixed _screen_ radius measured in points
 regardless of the map’s zoom level.
 
 The polygon that approximates an `MGLCircle` has a large number of vertices.
 If you do not need the circle to appear smooth at every possible zoom level,
 use a many-sided regular `MGLPolygon` instead for better performance.
 */
MGL_EXPORT
@interface MGLCircle : MGLShape

/**
 The coordinate around which the circle is centered.
 
 Each coordinate along the circle’s edge is equidistant from this coordinate.
 The center coordinate’s latitude helps determine the minimum spacing between
 each vertex along the edge of the polygon that approximates the circle.
 */
@property (nonatomic) CLLocationCoordinate2D coordinate;

/**
 The radius of the circular area, measured in meters across the Earth’s surface.
 */
@property (nonatomic) CLLocationDistance radius;

/**
 Creates and returns an `MGLCircle` object centered around the given coordinate
 and extending in all directions by the given physical distance.
 
 @param centerCoordinate The coordinate around which the circle is centered.
 @param radius The radius of the circular area, measured in meters across the
    Earth’s surface.
 @return A new circle object.
 */
+ (instancetype)circleWithCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate radius:(CLLocationDistance)radius;

/**
 Creates and returns an `MGLCircle` object that fills the given coordinate
 bounds.
 
 @param coordinateBounds The coordinate bounds to fill. The circle is centered
    around the center of the coordinate bounds. The circle’s edge touches at
    least two of the sides of the coordinate bounds. If the coordinate bounds
    does not represent a square area, the circle extends beyond two of its
    sides.
 @return A new circle object.
 */
+ (instancetype)circleWithCoordinateBounds:(MGLCoordinateBounds)coordinateBounds;

/**
 The smallest coordinate rectangle that completely encompasses the circle.
 
 If the circle spans the antimeridian, its bounds may extend west of −180
 degrees longitude or east of 180 degrees longitude. For example, a circle
 covering the Pacific Ocean from Tokyo to San Francisco might have a bounds
 extending from (35.68476, −220.24257) to (37.78428, −122.41310).
 */
@property (nonatomic, readonly) MGLCoordinateBounds coordinateBounds;

@end

NS_ASSUME_NONNULL_END
