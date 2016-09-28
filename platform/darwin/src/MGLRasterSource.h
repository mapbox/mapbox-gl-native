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
 
 @see <a href="https://www.mapbox.com/help/an-open-platform/#tilejson">The 
 TileJSON specification.</a>
 */
@property (nonatomic, readonly, copy) NSURL *URL;

/**
 The minimum visual size to display tiles for this source. Units in pixels. 
 Defaults to `512` on each tile side.
 */
@property (nonatomic, readonly, assign) NSUInteger tileSize;

/**
 The tile set used to locate and download tiles. 
 
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
 @param tileSet A tile set describing where to download tiles.
 @param tileSize The minimum visual size to display tiles for the source.
 */
- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileSet:(MGLTileSet *)tileSet tileSize:(CGFloat)tileSize;

@end

NS_ASSUME_NONNULL_END
