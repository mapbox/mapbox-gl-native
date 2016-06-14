#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLShape.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLMultiPoint` class is an abstract superclass used to define shapes
 composed of multiple points. You should not create instances of this class
 directly. Instead, you should create instances of the `MGLPolyline` or
 `MGLPolygon` classes. However, you can use the method and properties of this
 class to access information about the specific points associated with the line
 or polygon.
 */
@interface MGLMultiPoint : MGLShape

/** The array of coordinates associated with the shape. */
@property (nonatomic, readonly) CLLocationCoordinate2D *coordinates NS_RETURNS_INNER_POINTER;

/** The number of coordinates associated with the shape. (read-only) */
@property (nonatomic, readonly) NSUInteger pointCount;

/**
 Retrieves one or more coordinates associated with the shape.
 
 @param coords On input, you must provide a C array of structures large enough
    to hold the desired number of coordinates. On output, this structure
    contains the requested coordinate data.
 @param range The range of points you want. The `location` field indicates the
    first point you are requesting, with `0` being the first point, `1` being
    the second point, and so on. The `length` field indicates the number of
    points you want. The array in _`coords`_ must be large enough to accommodate
    the number of requested coordinates.
 */
- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range;

@end

NS_ASSUME_NONNULL_END
