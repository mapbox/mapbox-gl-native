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
 Creates and returns an `MGLPolygon` object from the specified set of
 coordinates.
 
 @param coords The array of coordinates defining the shape. The data in this
    array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new polygon object.
 */
+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count;

@end

/**
 The `MGLMultiPolygon` class represents a shape consisting of one or more
 polygons that do not overlap. For example, you would use an `MGLMultiPolygon`
 object to represent an atoll together with an island in the atoll’s lagoon:
 the atoll itself would be one `MGLPolygon` object, while the inner island would
 be another.
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
