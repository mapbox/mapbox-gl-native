#import "MGLSource.h"
#import "MGLTypes.h"

@class MGLTileSet;

NS_ASSUME_NONNULL_BEGIN

/**
 `MGLVectorSource` is a map content source that supplies tiled vector data in
 <a href="https://www.mapbox.com/vector-tiles/">Mapbox Vector Tile</a> format to
 be shown on the map. The location of and metadata about the vector tiles are
 defined by either an `MGLTileSet` object or an external TileJSON resource. A
 vector source is added to an `MGLStyle` object along with an
 `MGLVectorStyleLayer` object. The vector style layer defines the appearance of
 any content supplied by the vector source.
 
 Within each vector tile, each geometric coordinate must lie between
 −1&nbsp;×&nbsp;<var>extent</var> and
 (<var>extent</var>&nbsp;×&nbsp;2)&nbsp;−&nbsp;1, inclusive. Any vector style
 layer initialized with a vector source must have a non-`nil` value in its
 `sourceLayerIdentifier` property.
 */
@interface MGLVectorSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a vector source initialized with an identifier and TileJSON URL.
 
 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`).
 
 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param url A URL to a TileJSON configuration file describing the source’s
    contents and other metadata.
 @return An initialized vector source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url NS_DESIGNATED_INITIALIZER;

/**
 Returns a vector source initialized an identifier and tile set.

 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param tileSet A tile set describing the source’s contents and other metadata.
 @return An initialized vector source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier tileSet:(MGLTileSet *)tileSet NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing a Source’s Content

/**
 A URL to a TileJSON configuration file describing the source’s contents and
 other metadata.
 
 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`).
 
 If the receiver was initialized using `-initWithIdentifier:tileSet:`, this
 property is set to `nil`.
 */
@property (nonatomic, readonly, copy) NSURL *URL;

/**
 A tile set describing the source’s contents and other metadata.
 
 If the receiver was initialized using `-initWithIdentifier:URL:`, this property
 is set to `nil`.
 */
@property (nonatomic, readonly, nullable) MGLTileSet *tileSet;

@end

NS_ASSUME_NONNULL_END
