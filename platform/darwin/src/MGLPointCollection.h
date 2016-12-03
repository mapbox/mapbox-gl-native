#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLOverlay.h"
#import "MGLShape.h"

/**
 The `MGLPointCollection` class is used to define an array of disconnected 
 coordinates. The points in the collection may be related but are not 
 connected visually in any way.
 
 @note `MGLPointCollection` objects cannot be added to a map view using
 `-[MGLMapView addAnnotations:]` and related methods. However, when used in a
 `MGLPointCollectionFeature` to initialize a `MGLGeoJSONSource` that is added
 to the map view's style, the point collection represents as a group of distinct 
 annotations.
 */
@interface MGLPointCollection : MGLShape <MGLOverlay>

/**
 Creates and returns a `MGLPointCollection` object from the specified set of
 coordinates.
 
 @param coords The array of coordinates defining the shape. The data in this
 array is copied to the new object.
 @param count The number of items in the `coords` array.
 @return A new point collection object.
 */
+ (instancetype)pointCollectionWithCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count;

/** The array of coordinates associated with the shape. */
@property (nonatomic, readonly) CLLocationCoordinate2D *coordinates NS_RETURNS_INNER_POINTER;

/** The number of coordinates associated with the shape. */
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
