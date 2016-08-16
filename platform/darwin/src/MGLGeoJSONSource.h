#import "MGLSource.h"

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

@interface MGLGeoJSONSource : MGLSource

/**
 The contents of the source.
 
 If the receiver was initialized using `-initWithSourceIdentifier:URL:`, this
 property is set to `nil`. This property is unavailable until the receiver is
 passed into `-[MGLStyle addSource]`.
 */
@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLFeature>) *features;

/**
 A GeoJSON representation of the contents of the source.
 
 Use the `features` property instead to get an object representation of the
 contents. Alternatively, use NSJSONSerialization with the value of this
 property to transform it into Foundation types.
 
 If the receiver was initialized using `-initWithSourceIdentifier:URL:`, this
 property is set to `nil`. This property is unavailable until the receiver is
 passed into `-[MGLStyle addSource]`.
 */
@property (nonatomic, readonly, nullable, copy) NSData *geoJSONData;

/**
 The URL to the GeoJSON document that specifies the contents of the source.
 
 If the receiver was initialized using `-initWithSourceIdentifier:geoJSONData:`,
 this property is set to `nil`.
 */
@property (nonatomic, readonly, nullable) NSURL *URL;

/**
 Initializes a source with the given identifier and GeoJSON data.
 
 @param sourceIdentifier A string that uniquely identifies the source.
 @param geoJSONData An NSData object representing GeoJSON source code.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier geoJSONData:(NSData *)data NS_DESIGNATED_INITIALIZER;

/**
 Initializes a source with the given identifier and URL.
 
 @param sourceIdentifier A string that uniquely identifies the source.
 @param URL An HTTP(S) URL, absolute file URL, or local file URL relative to the
    current application’s resource bundle.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
