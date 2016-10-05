#import "MGLSource.h"
#import "MGLTypes.h"

@class MGLTileSet;

NS_ASSUME_NONNULL_BEGIN

/**
 A vector tile source. Tiles must be in Mapbox Vector Tile format.
 
 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector">The
    style specification.</a>
 */
@interface MGLVectorSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a vector source initialized with an identifier and TileJSON URL.
 
 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
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
 
 The URL may be a full HTTP or HTTPS URL or a Mapbox URL indicating the tile
 set’s map ID (`mapbox://<mapid>`).
 
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
