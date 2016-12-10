#import "MGLSource.h"
#import "MGLTypes.h"

#import <CoreGraphics/CoreGraphics.h>

@class MGLTileSet;

NS_ASSUME_NONNULL_BEGIN

/**
 `MGLRasterSource` is a map content source that supplies raster image tiles to
 be shown on the map. The location of and metadata about the raster tiles are
 defined by either an `MGLTileSet` object or an external TileJSON resource. A
 raster source is added to an `MGLStyle` object along with an
 `MGLRasterStyleLayer` object. Use a raster style layer to control the
 appearance of content supplied by the raster source.
 */
@interface MGLRasterSource : MGLSource

#pragma mark Initializing a Source

/**
 Returns a raster source initialized with an identifier, TileJSON configuration
 URL, and tile size.
 
 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`).
 
 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param url A URL to a TileJSON configuration file describing the source’s
    contents and other metadata.
 @param tileSize The height and width (measured in points) at which to display
    each tile in this source when the map’s zoom level is an integer.
 @return An initialized raster source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url tileSize:(CGFloat)tileSize NS_DESIGNATED_INITIALIZER;

/**
 Returns a raster source initialized with the given identifier, tile size, and
 tile set.
 
 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.

 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param tileSet A tile set describing the source’s contents and other metadata.
 @param tileSize The height and width (measured in points) at which to display
    each tile in this source when the map’s zoom level is an integer.
 @return An initialized raster source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier tileSet:(MGLTileSet *)tileSet tileSize:(CGFloat)tileSize NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing a Source’s Content

/**
 A URL to a TileJSON configuration file describing the source’s contents and
 other metadata.
 
 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`).
 
 @see <a href="https://www.mapbox.com/help/an-open-platform/#tilejson">The 
 TileJSON specification.</a>
 */
@property (nonatomic, readonly, copy) NSURL *URL;

/**
 The height and width (measured in points) at which to display each tile in this
 source when the map’s zoom level is an integer.
 
 A tile may be scaled up or down when the zoom level is between two integers.
 
 The default value of this property is 512 points.
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

@end

NS_ASSUME_NONNULL_END
