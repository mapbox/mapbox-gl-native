#import "MGLSource.h"

#import "MGLFoundation.h"
#import "MGLTypes.h"
#import "MGLShape.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

/**
 Options for `MGLShapeSource` objects.
 */
typedef NSString *MGLShapeSourceOption NS_STRING_ENUM;

/**
 An `NSNumber` object containing a Boolean enabling or disabling clustering.
 If the `shape` property contains point shapes, setting this option to
 `YES` clusters the points by radius into groups. The default value is `NO`.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-cluster"><code>cluster</code></a>
 source property in the Mapbox Style Specification.

 This option only affects point features within a shape source.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionClustered;

/**
 An `NSNumber` object containing an integer; specifies the radius of each
 cluster if clustering is enabled. A value of 512 produces a radius equal to
 the width of a tile. The default value is 50.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionClusterRadius;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to cluster points if clustering is enabled. Defaults to one zoom level
 less than the value of `MGLShapeSourceOptionMaximumZoomLevel` so that, at the
 maximum zoom level, the shapes are not clustered.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-clusterMaxZoom"><code>clusterMaxZoom</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevelForClustering;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to create vector tiles. A greater value produces greater detail at high
 zoom levels. The default value is 18.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-maxzoom"><code>maxzoom</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the size of the tile
 buffer on each side. A value of 0 produces no buffer. A value of 512 produces a
 buffer as wide as the tile itself. Larger values produce fewer rendering
 artifacts near tile edges and slower performance. The default value is 128.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-buffer"><code>buffer</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionBuffer;

/**
 An `NSNumber` object containing a double; specifies the Douglas-Peucker
 simplification tolerance. A greater value produces simpler geometries and
 improves performance. The default value is 0.375.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-tolerance"><code>tolerance</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionSimplificationTolerance;

/**
 `MGLShapeSource` is a map content source that supplies vector shapes to be
 shown on the map. The shapes may be instances of `MGLShape` or `MGLFeature`,
 or they may be defined by local or external
 <a href="http://geojson.org/">GeoJSON</a> code. A shape source is added to an
 `MGLStyle` object along with an `MGLVectorStyleLayer` object. The vector style
 layer defines the appearance of any content supplied by the shape source. You
 can update a shape source by setting its `shape` or `URL` property.

 Each
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson"><code>geojson</code></a>
 source defined by the style JSON file is represented at runtime by an
 `MGLShapeSource` object that you can use to refine the map’s content and
 initialize new style layers. You can also add and remove sources dynamically
 using methods such as `-[MGLStyle addSource:]` and
 `-[MGLStyle sourceWithIdentifier:]`.

 Any vector style layer initialized with a shape source should have a `nil`
 value in its `sourceLayerIdentifier` property.

 ### Example

 ```swift
 var coordinates: [CLLocationCoordinate2D] = [
     CLLocationCoordinate2D(latitude: 37.77, longitude: -122.42),
     CLLocationCoordinate2D(latitude: 38.91, longitude: -77.04),
 ]
 let polyline = MGLPolylineFeature(coordinates: &coordinates, count: UInt(coordinates.count))
 let source = MGLShapeSource(identifier: "lines", features: [polyline], options: nil)
 mapView.style?.addSource(source)
 ```
 */
MGL_EXPORT
@interface MGLShapeSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a shape source with an identifier, URL, and dictionary of options for
 the source.

 @param identifier A string that uniquely identifies the source.
 @param url An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a shape source with an identifier, a shape, and dictionary of options
 for the source.

 To specify attributes about the shape, use an instance of an `MGLShape`
 subclass that conforms to the `MGLFeature` protocol, such as `MGLPointFeature`.
 To include multiple shapes in the source, use an `MGLShapeCollection` or
 `MGLShapeCollectionFeature` object, or use the
 `-initWithIdentifier:features:options:` or
 `-initWithIdentifier:shapes:options:` methods.

 To create a shape from GeoJSON source code, use the
 `+[MGLShape shapeWithData:encoding:error:]` method.

 @param identifier A string that uniquely identifies the source.
 @param shape A concrete subclass of `MGLShape`
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier shape:(nullable MGLShape *)shape options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a shape source with an identifier, an array of features, and a dictionary
 of options for the source.

 Unlike `-initWithIdentifier:shapes:options:`, this method accepts `MGLFeature`
 instances, such as `MGLPointFeature` objects, whose attributes you can use when
 applying a predicate to `MGLVectorStyleLayer` or configuring a style layer’s
 appearance.

 To create a shape from GeoJSON source code, use the
 `+[MGLShape shapeWithData:encoding:error:]` method.

 @param identifier A string that uniquely identifies the source.
 @param features An array of objects that conform to the MGLFeature protocol.
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier features:(NS_ARRAY_OF(MGLShape<MGLFeature> *) *)features options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options;

/**
 Returns a shape source with an identifier, an array of shapes, and a dictionary of
 options for the source.

 Any `MGLFeature` instance passed into this initializer is treated as an ordinary
 shape, causing any attributes to be inaccessible to an `MGLVectorStyleLayer` when
 evaluating a predicate or configuring certain layout or paint attributes. To
 preserve the attributes associated with each feature, use the
 `-initWithIdentifier:features:options:` method instead.

 To create a shape from GeoJSON source code, use the
 `+[MGLShape shapeWithData:encoding:error:]` method.

 @param identifier A string that uniquely identifies the source.
 @param shapes An array of shapes; each shape is a member of a concrete subclass of MGLShape.
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier shapes:(NS_ARRAY_OF(MGLShape *) *)shapes options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options;

#pragma mark Accessing a Source’s Content

/**
 The contents of the source. A shape can represent a GeoJSON geometry, a
 feature, or a collection of features.

 If the receiver was initialized using `-initWithIdentifier:URL:options:`, this
 property is set to `nil`. This property is unavailable until the receiver is
 passed into `-[MGLStyle addSource:]`.

 You can get/set the shapes within a collection via this property. Actions must
 be performed on the application's main thread.
 */
@property (nonatomic, copy, nullable) MGLShape *shape;

/**
 The URL to the GeoJSON document that specifies the contents of the source.

 If the receiver was initialized using `-initWithIdentifier:shape:options:`,
 this property is set to `nil`.
 */
@property (nonatomic, copy, nullable) NSURL *URL;

/**
 Returns an array of map features for this source, filtered by the given
 predicate.

 Each object in the returned array represents a feature for the current style
 and provides access to attributes specified via the `shape` property.

 Features come from tiled GeoJSON data that is converted to tiles internally,
 so feature geometries are clipped at tile boundaries and features
 may appear duplicated across tiles. For example, suppose this source contains a
 long polyline representing a road. The resulting array includes those parts of
 the road that lie within the map tiles that the source has loaded, even if the
 road extends into other tiles. The portion of the road within each map tile is
 included individually.
 
 Returned features may not necessarily be visible to the user at the time they
 are loaded: the style may lack a layer that draws the features in question. To
 obtain only _visible_ features, use the
 `-[MGLMapView visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:predicate:]`
 or
 `-[MGLMapView visibleFeaturesInRect:inStyleLayersWithIdentifiers:predicate:]`
 method.

 @param predicate A predicate to filter the returned features. Use `nil` to
    include all features in the source.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the source that match the predicate.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)featuresMatchingPredicate:(nullable NSPredicate *)predicate;

@end

NS_ASSUME_NONNULL_END
