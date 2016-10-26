#import "MGLSource.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

/**
 An `NSNumber` object containing a Boolean enabling or disabling clustering.
 If the `features` property contains point features, setting this option to
 `YES` clusters the points by radius into groups. The default value is `NO`.
 */
extern NSString * const MGLGeoJSONClusterOption;

/**
 An `NSNumber` object containing an integer; specifies the radius of each
 cluster if clustering is enabled. A value of 512 produces a radius equal to
 the width of a tile. The default value is 50.
 */
extern NSString * const MGLGeoJSONClusterRadiusOption;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to cluster points if clustering is enabled. Defaults to one zoom level 
 less than the value of `MGLGeoJSONMaximumZoomLevelOption` so that, at the 
 maximum zoom level, the features are not clustered.
 */
extern NSString * const MGLGeoJSONClusterMaximumZoomLevelOption;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to create vector tiles. A greater value produces greater detail at high
 zoom levels. The default value is 18.
 */
extern NSString * const MGLGeoJSONMaximumZoomLevelOption;

/**
 An `NSNumber` object containing an integer; specifies the size of the tile
 buffer on each side. A value of 0 produces no buffer. A value of 512 produces a 
 buffer as wide as the tile itself. Larger values produce fewer rendering 
 artifacts near tile edges and slower performance. The default value is 128.
 */
extern NSString * const MGLGeoJSONBufferOption;

/**
 An `NSNumber` object containing a double; specifies the Douglas-Peucker
 simplification tolerance. A greater value produces simpler geometries and
 improves performance. The default value is 0.375.
 */
extern NSString * const MGLGeoJSONToleranceOption;

/**
 A GeoJSON source.

 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">The
    style specification.</a>
 */
@interface MGLGeoJSONSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a GeoJSON source initialized with an identifier, GeoJSON data, and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 @param identifier A string that uniquely identifies the source.
 @param geoJSONData An `NSData` object representing GeoJSON source code.
 @param options An `NSDictionary` of options for this source.
 @return An initialized GeoJSON source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier geoJSONData:(NSData *)data options:(nullable NS_DICTIONARY_OF(NSString *, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a GeoJSON source with an identifier, URL, and dictionary of options for
 the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 @param identifier A string that uniquely identifies the source.
 @param URL An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 @param options An `NSDictionary` of options for this source.
 @return An initialized GeoJSON source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(nullable NS_DICTIONARY_OF(NSString *, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a GeoJSON source with an identifier, features dictionary, and dictionary 
 of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 @param identifier A string that uniquely identifies the source.
 @param features An array of features that conform to the `MGLFeature` protocol.
 @param options An `NSDictionary` of options for this source.
 @return An initialized GeoJSON source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier features:(NSArray<id<MGLFeature>> *)features options:(nullable NS_DICTIONARY_OF(NSString *,id) *)options  NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing a Source’s Content

/**
 The contents of the source.

 If the receiver was initialized using `-initWithIdentifier:URL:options:`, this property
 is set to `nil`. This property is unavailable until the receiver is passed into
 `-[MGLStyle addSource]`.
 */
@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLFeature>) *features;

/**
 A GeoJSON representation of the contents of the source.
 
 Use the `features` property instead to get an object representation of the
 contents. Alternatively, use NSJSONSerialization with the value of this
 property to transform it into Foundation types.

 If the receiver was initialized using `-initWithIdentifier:URL:options` or 
 `-initWithIdentifier:features:options`, this property is set to `nil`. 
 This property is unavailable until the receiver is passed 
 into `-[MGLStyle addSource]`.
 */
@property (nonatomic, readonly, nullable, copy) NSData *geoJSONData;

/**
 The URL to the GeoJSON document that specifies the contents of the source.

 If the receiver was initialized using `-initWithIdentifier:geoJSONData:options`, this
 property is set to `nil`.
 */
@property (nonatomic, readonly, nullable) NSURL *URL;


@end

NS_ASSUME_NONNULL_END
