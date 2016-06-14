#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMultiPoint.h"
#import "MGLOverlay.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLPolygon` class represents a shape consisting of one or more points that
 define a closed polygon. The points are connected end-to-end in the order they
 are provided. The first and last points are connected to each other to create
 the closed shape.
 */
@interface MGLPolygon : MGLMultiPoint <MGLOverlay>

/**
 The array of polygons nested inside the receiver.
 
 The area occupied by any interior polygons is excluded from the overall shape.
 Interior polygons should not overlap. An interior polygon should not have
 interior polygons of its own.
 
 If there are no interior polygons, the value of this property is `nil`.
 */
@property (nonatomic, nullable, readonly) NS_ARRAY_OF(MGLPolygon *) *interiorPolygons;

/**
 Creates and returns an `MGLPolygon` object from the specified set of
 coordinates.
 
 @param coords The array of coordinates defining the shape. The data in this
    array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new polygon object.
 */
+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;

/**
 Creates and returns an `MGLPolygon` object from the specified set of
 coordinates and interior polygons.
 
 @param coords The array of coordinates defining the shape. The data in this
    array is copied to the new object.
 @param count The number of items in the `coords` array.
 @param interiorPolygons An array of `MGLPolygon` objects that define regions
    excluded from the overall shape. If this array is `nil` or empty, the shape
    is considered to have no interior polygons.
 @return A new polygon object.
 */
+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count interiorPolygons:(nullable NS_ARRAY_OF(MGLPolygon *) *)interiorPolygons;

@end

/**
 The `MGLMultiPolygon` class represents a shape consisting of one or more
 polygons that do not overlap. For example, you would use an `MGLMultiPolygon`
 object to represent an atoll together with an island in the atoll’s lagoon:
 the atoll itself would be one `MGLPolygon` object, while the inner island would
 be another.
 
 @note `MGLMultiPolygon` objects cannot be added to a map view using
    `-[MGLMapView addAnnotations:]` and related methods.
 */
@interface MGLMultiPolygon : MGLShape <MGLOverlay>

/**
 An array of polygons forming the multipolygon.
 */
@property (nonatomic, copy, readonly) NS_ARRAY_OF(MGLPolygon *) *polygons;

/**
 Creates and returns a multipolygon object consisting of the given polygons.
 
 @param polygons The array of polygons defining the shape.
 @return A new multipolygon object.
 */
+ (instancetype)multiPolygonWithPolygons:(NS_ARRAY_OF(MGLPolygon *) *)polygons;

@end

NS_ASSUME_NONNULL_END
