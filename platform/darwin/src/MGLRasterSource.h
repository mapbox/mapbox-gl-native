#import "MGLSource.h"
#import "MGLTypes.h"

#import <CoreGraphics/CoreGraphics.h>

@class MGLTileSet;

NS_ASSUME_NONNULL_BEGIN

/**
 A raster tile source.
 
 @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-raster">The
 style specification.</a>
 */
@interface MGLRasterSource : MGLSource

/**
 A URL to a TileJSON resource. Supported protocols are `http:`, `https:`, and 
 `mapbox://<mapid>`.
 */
@property (nonatomic, readonly, copy) NSURL *URL;

/**
 The minimum visual size to display tiles for this source. 
 */
@property (nonatomic, readonly, assign) NSUInteger tileSize;

/**
 A tile set holds the raster tile URL template strings and associated
 configuration for those strings. It can be passed in place of the URL
 to TileJSON in order to create a source configured to download tiles
 from ordinary web URLs.
 */
@property (nonatomic, readonly, nullable) MGLTileSet *tileSet;

/**
 Initializes a source with the given identifier, TileJSON configuration
 URL, and tile size.

 @param sourceIdentifier A string that uniquely identifies the source.
 @param url A URL to a TileJSON resource.
 @param tileSize The minimum visual size to display tiles for the source.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url tileSize:(CGFloat)tileSize;

/**
 Initializes a source with the given identifier, tile size, and tile 
 URL template set.

 @param sourceIdentifier A string that uniquely identifies the source.
 @param tileSize The minimum visual size to display tiles for the source.
 @param tileSet A tile set describing where to download tiles.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileSize:(CGFloat)tileSize tileSet:(MGLTileSet *)tileSet;

@end

NS_ASSUME_NONNULL_END
