#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"
#import "MGLMultiPoint.h"
#import "MGLOverlay.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An `MGLPolyline` object represents a shape consisting of two or more vertices,
 specified as `CLLocationCoordinate2D` instances, and the line segments that
 connect them. For example, you could use an polyline to represent a road or the
 path along which something moves.

 You can add polyline shapes to the map by adding them to an `MGLShapeSource`
 object. Configure the appearance of an `MGLShapeSource`’s or
 `MGLVectorTileSource`’s polylines collectively using an `MGLLineStyleLayer` or
 `MGLSymbolStyleLayer` object. To access a polyline’s attributes, use an
 `MGLPolylineFeature` object.

 Alternatively, you can add a polyline overlay directly to a map view using the
 `-[MGLMapView addAnnotation:]` or `-[MGLMapView addOverlay:]` method. Configure
 a polyline overlay’s appearance using
 `-[MGLMapViewDelegate mapView:strokeColorForShapeAnnotation:]` and
 `-[MGLMapViewDelegate mapView:lineWidthForPolylineAnnotation:]`.

 The vertices are automatically connected in the order in which you provide
 them. The first and last vertices are not connected to each other, but you can
 specify the same `CLLocationCoordinate2D` as the first and last vertices in
 order to close the polyline. To fill the space within the shape, use an
 `MGLPolygon` object. To group multiple polylines together in one shape, use an
 `MGLMultiPolyline` or `MGLShapeCollection` object.

 To make the polyline go across the antimeridian or international date line, 
 specify some longitudes less than −180 degrees or greater than 180 degrees.
 For example, a polyline that stretches from Tokyo to San Francisco would have
 coordinates of (35.68476, -220.24257) and (37.78428, -122.41310).
 
 ```swift
 let coordinates = [
     CLLocationCoordinate2D(latitude: 35.68476, longitude: -220.24257),
     CLLocationCoordinate2D(latitude: 37.78428, longitude: -122.41310)
 ]
 let polyline = MGLPolyline(coordinates: coordinates, count: UInt(coordinates.count))
 ```

 A polyline is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.4">LineString</a>
 geometry in GeoJSON.
 */
MGL_EXPORT
@interface MGLPolyline : MGLMultiPoint <MGLOverlay>

/**
 Creates and returns an `MGLPolyline` object from the specified set of
 coordinates.

 @param coords The array of coordinates defining the shape. The data in this
    array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new polyline object.
 */
+ (instancetype)polylineWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count;

@end

/**
 An `MGLMultiPolyline` object represents a shape consisting of one or more
 polylines. For example, you could use a multipolyline shape to represent both
 sides of a divided highway (dual carriageway), excluding the median (central
 reservation): each carriageway would be a distinct `MGLPolyline` object.

 You can add multipolyline shapes to the map by adding them to an
 `MGLShapeSource` object. Configure the appearance of an `MGLShapeSource`’s or
 `MGLVectorTileSource`’s multipolylines collectively using an
 `MGLLineStyleLayer` or `MGLSymbolStyleLayer` object.

 You cannot add an `MGLMultiPolyline` object directly to a map view using
 `-[MGLMapView addAnnotation:]` or `-[MGLMapView addOverlay:]`. However, you can
 add the `polylines` array’s items as overlays individually.

 A multipolyline is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.5">MultiLineString</a>
 geometry in GeoJSON.
 */
MGL_EXPORT
@interface MGLMultiPolyline : MGLShape <MGLOverlay>

/**
 An array of polygons forming the multipolyline.
 */
@property (nonatomic, copy, readonly) NSArray<MGLPolyline *> *polylines;

/**
 Creates and returns a multipolyline object consisting of the given polylines.

 @param polylines The array of polylines defining the shape.
 @return A new multipolyline object.
 */
+ (instancetype)multiPolylineWithPolylines:(NSArray<MGLPolyline *> *)polylines;

@end

NS_ASSUME_NONNULL_END
