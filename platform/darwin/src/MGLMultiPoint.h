#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"
#import "MGLShape.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLMultiPoint` class is an abstract superclass used to define shapes
 composed of multiple vertices.

 Create instances of `MGLPolyline` or `MGLPolygon` in order to use 
 properties of `MGLMultiPoint`. Do not create instances of `MGLMultiPoint`
 directly and do not create your own subclasses of this class. You can use
 the method and properties of this class to access information about the
 vertices of the line or polygon.

 Do not confuse `MGLMultiPoint` with `MGLPointCollection`, which represents a
 collection of related but disconnected points.
 */
MGL_EXPORT
@interface MGLMultiPoint : MGLShape

/**
 The array of vertices associated with the shape.

 This C array is a pointer to a structure inside the multipoint object, which
 may have a lifetime shorter than the multipoint object and will certainly not
 have a longer lifetime. Therefore, you should copy the C array if it needs to
 be stored outside of the memory context in which you use this property.
 */
@property (nonatomic, readonly) CLLocationCoordinate2D *coordinates NS_RETURNS_INNER_POINTER;

/** The number of vertices in the shape. */
@property (nonatomic, readonly) NSUInteger pointCount;

/**
 Retrieves the vertices of part of the shape.

 @param coords On input, you must provide a C array of `CLLocationCoordinate2D`
    structures large enough to hold the desired number of coordinates. On
    output, this structure contains the requested coordinate data.
 @param range The range of vertices you want. The `location` field indicates
    the first vertex you are requesting, with `0` being the first vertex, `1`
    being the second vertex, and so on. The `length` field indicates the number
    of vertices you want. The array in `coords` must be large enough to
    accommodate the number of requested coordinates.
 */
- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range;

/**
 Sets the shape’s vertices to the given C array of vertices.

 @param coords The array of coordinates defining the shape. The data in this
    array is copied to the shape’s `coordinates` property.
 @param count The number of coordinates from the `coords` array.
 */
- (void)setCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;

/**
 Inserts the given vertices into the shape.
 
 If the shape is currently visible on the map as an annotation, it is redrawn
 immediately. If the shape is part of an `MGLShapeSource` object, you must
 explicitly set the `MGLShapeSource.shape` property in order for any style
 layers that use the source to be redrawn.

 @param coords The array of coordinates to insert into the shape. The data in
    this array is copied to the shape’s `coordinates` property.
 @param count The number of items in the `coords` array.
 @param index The zero-based index at which the first coordinate in `coords`
    will appear in the `coordinates` property.
 */
- (void)insertCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count atIndex:(NSUInteger)index;

/**
 Appends the given vertices to the shape.
 
 If the shape is currently visible on the map as an annotation, it is redrawn
 immediately. If the shape is part of an `MGLShapeSource` object, you must
 explicitly set the `MGLShapeSource.shape` property in order for any style
 layers that use the source to be redrawn.

 @param coords The array of coordinates to add to the shape. The data in this
    array is copied to the shape’s `coordinates` property.
 @param count The number of items in the `coords` array.
 */
- (void)appendCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count;

/**
 Replaces the vertices at the given range in the shape with the same number of
 vertices from a given C array.
 
 If the shape is currently visible on the map as an annotation, it is redrawn
 immediately. If the shape is part of an `MGLShapeSource` object, you must
 explicitly set the `MGLShapeSource.shape` property in order for any style
 layers that use the source to be redrawn.

 The number of coordinates in `coords` must be equal to the length of `range`.
 If you want to insert or delete one or more vertices, use the
 `-replaceCoordinatesInRange:withCoordinates:count:` method.

 If `range` extends beyond the shape’s `coordinates` property, an
 `NSRangeException` is raised. If you want to append new vertices to the shape,
 use the `-appendCoordinates:count:` method.

 @param range The range of vertices to replace. The `location` field indicates
    the first vertex you are replacing, with `0` being the first vertex, `1`
    being the second vertex, and so on. The `length` field indicates the number
    of vertices to replace.
 @param coords The array of coordinates defining part of the shape. The data in
    this array is copied to the shape’s `coordinates` property.
 */
- (void)replaceCoordinatesInRange:(NSRange)range withCoordinates:(const CLLocationCoordinate2D *)coords;

/**
 Replaces the vertices at the given range in the shape with the specified number
 of vertices from a given C array.
 
 If the shape is currently visible on the map as an annotation, it is redrawn
 immediately. If the shape is part of an `MGLShapeSource` object, you must
 explicitly set the `MGLShapeSource.shape` property in order for any style
 layers that use the source to be redrawn.

 If `count` is greater than the `length` field of `range`, some vertices will
 effectively be inserted into the shape. On the other hand, if `count` is less
 than the `length` field of `range`, some vertices will effectively be removed.

 If `range` extends beyond the shape’s `coordinates` property, an
 `NSRangeException` is raised. If you want to append new vertices to the shape,
 use the `-appendCoordinates:count:` method.

 @param range The range of vertices to replace. The `location` field indicates
    the first vertex you are replacing, with `0` being the first vertex, `1`
    being the second vertex, and so on. The `length` field indicates the number
    of vertices to replace.
 @param coords The array of coordinates defining part of the shape. The data in
    this array is copied to the shape’s `coordinates` property.
 @param count The number of coordinates from the `coords` array to insert in
    place of the coordinates in `range`. The sum of `range`’s length and this
    count must not exceed the number of items currently in the `coordinates`
    property.
 */
- (void)replaceCoordinatesInRange:(NSRange)range withCoordinates:(const CLLocationCoordinate2D *)coords count:(NSUInteger)count;

/**
 Removes the vertices at the given range from the shape.
 
 If the shape is currently visible on the map as an annotation, it is redrawn
 immediately. If the shape is part of an `MGLShapeSource` object, you must
 explicitly set the `MGLShapeSource.shape` property in order for any style
 layers that use the source to be redrawn.

 If `range` extends beyond the shape’s `coordinates` property, an
 `NSRangeException` is raised.

 @param range The range of vertices to remove. The `location` field indicates
    the first vertex you are removing, with `0` being the first vertex, `1`
    being the second vertex, and so on. The `length` field indicates the number
    of vertices to remove.
 */
- (void)removeCoordinatesInRange:(NSRange)range;

@end

NS_ASSUME_NONNULL_END
