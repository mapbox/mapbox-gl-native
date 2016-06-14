#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMultiPoint.h"
#import "MGLOverlay.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLPolyline` class represents a shape consisting of one or more points
 that define connecting line segments. The points are connected end-to-end in
 the order they are provided. The first and last points are not connected to
 each other.
 */
@interface MGLPolyline : MGLMultiPoint <MGLOverlay>

/**
 Creates and returns an `MGLPolyline` object from the specified set of
 coordinates.
 
 @param coords The array of coordinates defining the shape. The data in this
    array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new polyline object.
 */
+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count;

@end

/**
 The `MGLMultiPolyline` class represents a shape consisting of one or more
 polylines. For example, you could use an `MGLMultiPolyline` object to represent
 both sides of a divided highway (dual carriageway), excluding the median
 (central reservation): each carriageway would be a distinct `MGLPolyline`
 object.
 
 @note `MGLMultiPolyline` objects cannot be added to a map view using
    `-[MGLMapView addAnnotations:]` and related methods.
 */
@interface MGLMultiPolyline : MGLShape <MGLOverlay>

/**
 An array of polygons forming the multipolyline.
 */
@property (nonatomic, copy, readonly) NS_ARRAY_OF(MGLPolyline *) *polylines;

/**
 Creates and returns a multipolyline object consisting of the given polylines.
 
 @param polylines The array of polylines defining the shape.
 @return A new multipolyline object.
 */
+ (instancetype)multiPolylineWithPolylines:(NS_ARRAY_OF(MGLPolyline *) *)polylines;

@end

NS_ASSUME_NONNULL_END
