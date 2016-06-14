#import <Foundation/Foundation.h>

#import "MGLShape.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLShapeCollection` class represents a shape consisting of one or more
 distinct but related shapes that are instances of `MGLShape`. The constituent
 shapes can be a mixture of different kinds of shapes.
 
 @note `MGLShapeCollection` objects cannot be added to a map view using
    `-[MGLMapView addAnnotations:]` and related methods.
 */
@interface MGLShapeCollection : MGLShape

/**
 An array of shapes forming the shape collection.
 */
@property (nonatomic, copy, readonly) NS_ARRAY_OF(MGLShape *) *shapes;

/**
 Creates and returns a shape collection consisting of the given shapes.
 
 @param shapes The array of shapes defining the shape collection. The data in
    this array is copied to the new object.
 @return A new shape collection object.
 */
+ (instancetype)shapeCollectionWithShapes:(NS_ARRAY_OF(MGLShape *) *)shapes;

@end

NS_ASSUME_NONNULL_END
