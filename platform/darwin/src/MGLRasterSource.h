#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLTileSource.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An `NSNumber` object containing a floating-point number that specifies the
 width and height (measured in points) at which the map displays each raster
 image tile when the map’s zoom level is an integer. The raster source scales
 its images up or down when the map’s zoom level falls between two integers.

 The default value for this option is 512. Version 4 of the
 <a href="https://www.mapbox.com/api-documentation/#maps">Mapbox Maps API</a>
 requires a value of 256, as do many third-party tile servers, so consult your
 provider’s documentation for the correct value.

 This option is only applicable to `MGLRasterSource` objects; it is ignored when
 initializing `MGLVectorSource` objects.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionTileSize;

/**
 `MGLRasterSource` is a map content source that supplies raster image tiles to
 be shown on the map. The location of and metadata about the tiles are defined
 either by an option dictionary or by an external file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.
 A raster source is added to an `MGLStyle` object along with one or more
 `MGLRasterStyleLayer` objects. Use a raster style layer to control the
 appearance of content supplied by the raster source.

 Each
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-raster"><code>raster</code></a>
 source defined by the style JSON file is represented at runtime by an
 `MGLRasterSource` object that you can use to initialize new style layers. You
 can also add and remove sources dynamically using methods such as
 `-[MGLStyle addSource:]` and `-[MGLStyle sourceWithIdentifier:]`.

 ### Example

 ```swift
 let source = MGLRasterSource(identifier: "clouds", tileURLTemplates: ["https://example.com/raster-tiles/{z}/{x}/{y}.png"], options: [
     .minimumZoomLevel: 9,
     .maximumZoomLevel: 16,
     .tileSize: 512,
     .attributionInfos: [
         MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
     ]
 ])
 mapView.style?.addSource(source)
 ```
 */
MGL_EXPORT
@interface MGLRasterSource : MGLTileSource

#pragma mark Initializing a Source

/**
 Returns a raster source initialized with an identifier and configuration URL.

 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.

 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`). The URL should
 point to a JSON file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.

 If a Mapbox URL is specified, this source uses a tile size of 256. For all
 other tile sets, the default value is 512. (See the
 `MGLTileSourceOptionTileSize` documentation for more information about tile
 sizes.) If you need to use a tile size other than the default, use the
 `-initWithIdentifier:configurationURL:tileSize:` method.

 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param configurationURL A URL to a TileJSON configuration file describing the
    source’s contents and other metadata.
 @return An initialized raster source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL;

/**
 Returns a raster source initialized with an identifier, configuration URL, and
 tile size.

 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.

 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`). The URL should
 point to a JSON file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.

 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param configurationURL A URL to a TileJSON configuration file describing the
 source’s contents and other metadata.
 @param tileSize The width and height (measured in points) of each tiled image
    in the raster source. See the `MGLTileSourceOptionTileSize` documentation
    for details.
 @return An initialized raster source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL tileSize:(CGFloat)tileSize NS_DESIGNATED_INITIALIZER;

/**
 Returns a raster source initialized an identifier, tile URL templates, and
 options.

 Tile URL templates are strings that specify the URLs of the raster tile images
 to load. See the “<a href="../tile-url-templates.html">Tile URL Templates</a>”
 guide for information about the format of a tile URL template.

 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.

 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param tileURLTemplates An array of tile URL template strings. Only the first
    string is used; any additional strings are ignored.
 @param options A dictionary containing configuration options. See
    `MGLTileSourceOption` for available keys and values. Pass in `nil` to use
    the default values.
 @return An initialized tile source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
