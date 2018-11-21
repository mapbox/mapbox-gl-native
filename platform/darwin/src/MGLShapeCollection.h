#import <Foundation/Foundation.h>

#import "MGLFoundation.h"
#import "MGLShape.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An `MGLShapeCollection` object represents a shape consisting of zero or more
 distinct but related shapes that are instances of `MGLShape`. The constituent
 shapes can be a mixture of different kinds of shapes.

 `MGLShapeCollection` is most commonly used to add multiple shapes to a single
 `MGLShapeSource`. Configure the appearance of an `MGLShapeSource`’s or
 `MGLVectorTileSource`’s shape collection collectively using an
 `MGLSymbolStyleLayer` object, or use multiple instances of
 `MGLCircleStyleLayer`, `MGLFillStyleLayer`, and `MGLLineStyleLayer` to
 configure the appearance of each kind of shape inside the collection.

 You cannot add an `MGLShapeCollection` object directly to a map view as an
 annotation. However, you can create individual `MGLPointAnnotation`,
 `MGLPolyline`, and `MGLPolygon` objects from the `shapes` array and add those
 annotation objects to the map view using the `-[MGLMapView addAnnotations:]`
 method.

 To represent a collection of point, polyline, or polygon shapes, it may be more
 convenient to use an `MGLPointCollection`, `MGLMultiPolyline`, or
 `MGLMultiPolygon` object, respectively. To access a shape collection’s
 attributes, use the corresponding `MGLFeature` object.

 A shape collection is known as a
 <a href="https://tools.ietf.org/html/rfc7946#section-3.1.8">GeometryCollection</a>
 geometry in GeoJSON.
 */
MGL_EXPORT
@interface MGLShapeCollection : MGLShape

/**
 An array of shapes forming the shape collection.
 */
@property (nonatomic, copy, readonly) NSArray<MGLShape *> *shapes;

/**
 Creates and returns a shape collection consisting of the given shapes.

 @param shapes The array of shapes defining the shape collection. The data in
    this array is copied to the new object.
 @return A new shape collection object.
 */
+ (instancetype)shapeCollectionWithShapes:(NSArray<MGLShape *> *)shapes;

@end

NS_ASSUME_NONNULL_END
