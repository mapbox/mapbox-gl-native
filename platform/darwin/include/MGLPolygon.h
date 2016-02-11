#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLMultiPoint.h"
#import "MGLOverlay.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The `MGLPolygon` class represents a shape consisting of one or more points that define a closed polygon. The points are connected end-to-end in the order they are provided. The first and last points are connected to each other to create the closed shape. */
@interface MGLPolygon : MGLMultiPoint <MGLOverlay>

/**
 Creates and returns an `MGLPolygon` object from the specified set of coordinates.
 
 @param coords The array of coordinates defining the shape. The data in this array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new polygon object.
 */
+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count;

@end

NS_ASSUME_NONNULL_END
