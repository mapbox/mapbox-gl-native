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

 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.

 #### Tile URL templates

 Tile URL templates are strings that specify the URLs of the tile images to
 load. Each template resembles an absolute URL, but with any number of
 placeholder strings that the source evaluates based on the tile it needs to
 load. For example:

 <ul>
 <li><code>http://www.example.com/tiles/{z}/{x}/{y}.pbf</code> could be
    evaluated as <code>http://www.example.com/tiles/14/6/9.pbf</code>.</li>
 <li><code>http://www.example.com/tiles/{z}/{x}/{y}{ratio}.png</code> could be
    evaluated as <code>http://www.example.com/tiles/14/6/9@2x.png</code>.</li>
 </ul>

 Tile sources support the following placeholder strings in tile URL templates,
 all of which are optional:

 <table>
 <thead>
 <tr><th>Placeholder string</th><th>Description</th></tr>
 </thead>
 <tbody>
 <tr>
     <td><code>{x}</code></td>
     <td>The index of the tile along the map’s x axis according to Spherical
        Mercator projection. If the value is 0, the tile’s left edge corresponds
        to the 180th meridian west. If the value is 2<sup><var>z</var></sup>−1,
        the tile’s right edge corresponds to the 180th meridian east.</td>
 </tr>
 <tr>
     <td><code>{y}</code></td>
     <td>The index of the tile along the map’s y axis according to Spherical
        Mercator projection. If the value is 0, the tile’s tile edge corresponds
        to arctan(sinh(π)), or approximately 85.0511 degrees north. If the value
        is 2<sup><var>z</var></sup>−1, the tile’s bottom edge corresponds to
        −arctan(sinh(π)), or approximately 85.0511 degrees south. The y axis is
        inverted if the <code>options</code> parameter contains
        <code>MGLTileSourceOptionTileCoordinateSystem</code> with a value of
        <code>MGLTileCoordinateSystemTMS</code>.</td>
 </tr>
 <tr>
     <td><code>{z}</code></td>
     <td>The tile’s zoom level. At zoom level 0, each tile covers the entire
        world map; at zoom level 1, it covers ¼ of the world; at zoom level 2,
        <sup>1</sup>⁄<sub>16</sub> of the world, and so on. For tiles loaded by
        a <code>MGLRasterSource</code> object, whether the tile zoom level
        matches the map’s current zoom level depends on the value of the
        source’s tile size as specified in the
        <code>MGLTileSourceOptionTileSize</code> key of the
        <code>options</code> parameter.</td>
 </tr>
 <tr>
     <td><code>{bbox-epsg-3857}</code></td>
     <td>The tile’s bounding box, expressed as a comma-separated list of the
        tile’s western, southern, eastern, and northern extents according to
        Spherical Mercator (EPSG:3857) projection. The bounding box is typically
        used with map services conforming to the
        <a href="http://www.opengeospatial.org/standards/wms">Web Map Service</a>
        protocol.</td>
 </tr>
 <tr>
     <td><code>{quadkey}</code></td>
     <td>A quadkey indicating both the tile’s location and its zoom level. The
        quadkey is typically used with
        <a href="https://msdn.microsoft.com/en-us/library/bb259689.aspx">Bing Maps</a>.
     </td>
 </tr>
 <tr>
     <td><code>{ratio}</code></td>
     <td>A suffix indicating the resolution of the tile image. The suffix is the
        empty string for standard resolution displays and <code>@2x</code> for
        Retina displays, including displays for which
        <code>NSScreen.backingScaleFactor</code> or <code>UIScreen.scale</code>
        is 3.</td>
 </tr>
 <tr>
     <td><code>{prefix}</code></td>
     <td>Two hexadecimal digits chosen such that each visible tile has a
        different prefix. The prefix is typically used for domain sharding.</td>
 </tr>
 </tbody>
 </table>

 For more information about the `{x}`, `{y}`, and `{z}` placeholder strings,
 consult the
 <a href="https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames">OpenStreetMap Wiki</a>.

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
