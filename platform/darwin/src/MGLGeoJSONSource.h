#import "MGLSource.h"

#import "MGLTypes.h"
#import "MGLGeoJSONSourceBase.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

/**
 A GeoJSON source.

 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">The
    style specification.</a>
 */
@interface MGLGeoJSONSource : MGLGeoJSONSourceBase

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
- (instancetype)initWithIdentifier:(NSString *)identifier geoJSONData:(NSData *)data options:(nullable NS_DICTIONARY_OF(MGLGeoJSONSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

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
- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(nullable NS_DICTIONARY_OF(MGLGeoJSONSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

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
- (instancetype)initWithIdentifier:(NSString *)identifier features:(NSArray<id<MGLFeature>> *)features options:(nullable NS_DICTIONARY_OF(MGLGeoJSONSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing a Source’s Content

/**
 The contents of the source.

 If the receiver was initialized using `-initWithIdentifier:URL:options:`, this property
 is set to `nil`. This property is unavailable until the receiver is passed into
 `-[MGLStyle addSource]`.
 */
@property (nonatomic, nullable) NS_ARRAY_OF(id <MGLFeature>) *features;

/**
 A GeoJSON representation of the contents of the source.
 
 Use the `features` property instead to get an object representation of the
 contents. Alternatively, use `NSJSONSerialization` with the value of this
 property to transform it into Foundation types.

 If the receiver was initialized using `-initWithIdentifier:URL:options` or 
 `-initWithIdentifier:features:options`, this property is set to `nil`. 
 This property is unavailable until the receiver is passed 
 into `-[MGLStyle addSource]`.
 */
@property (nonatomic, nullable, copy) NSData *geoJSONData;

/**
 The URL to the GeoJSON document that specifies the contents of the source.

 If the receiver was initialized using `-initWithIdentifier:geoJSONData:options`, this
 property is set to `nil`.
 */
@property (nonatomic, nullable) NSURL *URL;


@end

NS_ASSUME_NONNULL_END
