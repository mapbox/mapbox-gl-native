#import "MGLSource.h"

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
 */
extern const MGLShapeSourceOption MGLShapeSourceOptionClustered;

/**
 An `NSNumber` object containing an integer; specifies the radius of each
 cluster if clustering is enabled. A value of 512 produces a radius equal to
 the width of a tile. The default value is 50.
 */
extern const MGLShapeSourceOption MGLShapeSourceOptionClusterRadius;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to cluster points if clustering is enabled. Defaults to one zoom level 
 less than the value of `MGLShapeSourceOptionMaximumZoomLevel` so that, at the
 maximum zoom level, the shapes are not clustered.
 */
extern const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevelForClustering;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to create vector tiles. A greater value produces greater detail at high
 zoom levels. The default value is 18.
 */
extern const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the size of the tile
 buffer on each side. A value of 0 produces no buffer. A value of 512 produces a 
 buffer as wide as the tile itself. Larger values produce fewer rendering 
 artifacts near tile edges and slower performance. The default value is 128.
 */
extern const MGLShapeSourceOption MGLShapeSourceOptionBuffer;

/**
 An `NSNumber` object containing a double; specifies the Douglas-Peucker
 simplification tolerance. A greater value produces simpler geometries and
 improves performance. The default value is 0.375.
 */
extern const MGLShapeSourceOption MGLShapeSourceOptionSimplificationTolerance;

/**
 A shape source.

 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">The
    style specification.</a>
 */
@interface MGLShapeSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a shape source initialized with an identifier, GeoJSON data, and a 
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 @param identifier A string that uniquely identifies the source.
 @param geoJSONData An `NSData` object representing GeoJSON source code.
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier geoJSONData:(NSData *)data options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a shape source with an identifier, URL, and dictionary of options for
 the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 @param identifier A string that uniquely identifies the source.
 @param URL An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a shape source with an identifier, a shape, and dictionary
 of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 @param identifier A string that uniquely identifies the source.
 @param shape A concrete subclass of `MGLShape`
 @param options An `NSDictionary` of options for this source.
 @return An initialized shape source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier shape:(nullable MGLShape *)shape options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing a Source’s Content

/**
 The contents of the source. A shape can represent a GeoJSON geometry, a feature, 
 or a collection of features.

 If the receiver was initialized using `-initWithIdentifier:URL:options:`, this property
 is set to `nil`. This property is unavailable until the receiver is passed into
 `-[MGLStyle addSource]`.
 */
@property (nonatomic, nullable) MGLShape *shape;

/**
 A GeoJSON representation of the contents of the source.
 
 Use the `shape` property instead to get an object representation of the
 contents. Alternatively, use `NSJSONSerialization` with the value of this
 property to transform it into Foundation types.

 If the receiver was initialized using `-initWithIdentifier:URL:options` or 
 `-initWithIdentifier:shape:options`, this property is set to `nil`.
 This property is unavailable until the receiver is passed  into 
 `-[MGLStyle addSource]`.
 */
@property (nonatomic, nullable, copy) NSData *geoJSONData;

/**
 The URL to the GeoJSON document that specifies the contents of the source.

 If the receiver was initialized using `-initWithIdentifier:geoJSONData:options`, 
 this property is set to `nil`.
 */
@property (nonatomic, nullable) NSURL *URL;

@end

NS_ASSUME_NONNULL_END
