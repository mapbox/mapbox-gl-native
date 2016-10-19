#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"

/**
 The `MGLPointCollection` class is used to define shapes composed of multiple 
 points.
 */
@interface MGLPointCollection : MGLShape

/**
 Creates and returns a `MGLPointCollection` object from the specified set of
 coordinates.
 
 @param coords The array of coordinates defining the shape. The data in this
 array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new multipoint object.
 */
+ (instancetype)pointCollectionWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;

/** The array of coordinates associated with the shape. */
@property (nonatomic, readonly) CLLocationCoordinate2D *coordinates NS_RETURNS_INNER_POINTER;

/** The number of coordinates associated with the shape. */
@property (nonatomic, readonly) NSUInteger pointCount;

@end
