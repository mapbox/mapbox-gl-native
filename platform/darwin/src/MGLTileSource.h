#import <Foundation/Foundation.h>

#import "MGLFoundation.h"
#import "MGLSource.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAttributionInfo;

/**
 Options for `MGLTileSource` objects.
 */
typedef NSString *MGLTileSourceOption NS_STRING_ENUM;

/**
 An `NSNumber` object containing an unsigned integer that specifies the minimum
 zoom level at which to display tiles from the source.

 The value should be between 0 and 22, inclusive, and less than
 `MGLTileSourceOptionMaximumZoomLevel`, if specified. The default value for this
 option is 0.

 This option corresponds to the `minzoom` key in the
 <a href="https://github.com/mapbox/tilejson-spec/tree/master/2.1.0">TileJSON</a>
 specification.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionMinimumZoomLevel;

/**
 An `NSNumber` object containing an unsigned integer that specifies the maximum
 zoom level at which to display tiles from the source.

 The value should be between 0 and 22, inclusive, and less than
 `MGLTileSourceOptionMinimumZoomLevel`, if specified. The default value for this
 option is 22.

 This option corresponds to the `maxzoom` key in the
 <a href="https://github.com/mapbox/tilejson-spec/tree/master/2.1.0">TileJSON</a>
 specification.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionMaximumZoomLevel;

#if TARGET_OS_IPHONE
/**
 An HTML string defining the buttons to be displayed in an action sheet when the
 source is part of a map view’s style and the map view’s attribution button is
 pressed.

 By default, no attribution statements are displayed. If the
 `MGLTileSourceOptionAttributionInfos` option is specified, this option is
 ignored.

 This option corresponds to the `attribution` key in the
 <a href="https://github.com/mapbox/tilejson-spec/tree/master/2.1.0">TileJSON</a>
 specification.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionAttributionHTMLString;

/**
 An array of `MGLAttributionInfo` objects defining the buttons to be displayed
 in an action sheet when the source is part of a map view’s style and the map
 view’s attribution button is pressed.

 By default, no attribution statements are displayed.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionAttributionInfos;
#else
/**
 An HTML string defining the buttons to be displayed in the map view’s
 attribution view when the source is part of the map view’s style.

 By default, no attribution statements are displayed. If the
 `MGLTileSourceOptionAttributionInfos` option is specified, this option is
 ignored.

 This option corresponds to the `attribution` key in the
 <a href="https://github.com/mapbox/tilejson-spec/tree/master/2.1.0">TileJSON</a>
 specification.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionAttributionHTMLString;

/**
 An array of `MGLAttributionInfo` objects defining the buttons to be displayed
 in the map view’s attribution view when the source is part of the map view’s
 style.

 By default, no attribution statements are displayed.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionAttributionInfos;
#endif

/**
 An `NSNumber` object containing an unsigned integer that specifies the tile
 coordinate system for the source’s tile URLs. The integer corresponds to one of
 the constants described in `MGLTileCoordinateSystem`.

 The default value for this option is `MGLTileCoordinateSystemXYZ`.

 This option corresponds to the `scheme` key in the
 <a href="https://github.com/mapbox/tilejson-spec/tree/master/2.1.0">TileJSON</a>
 specification.
 */
extern MGL_EXPORT const MGLTileSourceOption MGLTileSourceOptionTileCoordinateSystem;

/**
 Tile coordinate systems that determine how tile coordinates in tile URLs are
 interpreted.
 */
typedef NS_ENUM(NSUInteger, MGLTileCoordinateSystem) {
    /**
     The origin is at the top-left (northwest), and `y` values increase
     southwards.

     This tile coordinate system is used by Mapbox and OpenStreetMap tile
     servers.
     */
    MGLTileCoordinateSystemXYZ = 0,

    /**
     The origin is at the bottom-left (southwest), and `y` values increase
     northwards.

     This tile coordinate system is used by tile servers that conform to the
     <a href="http://wiki.osgeo.org/wiki/Tile_Map_Service_Specification">Tile Map Service Specification</a>.
     */
    MGLTileCoordinateSystemTMS
};

/**
 `MGLTileSource` is a map content source that supplies map tiles to be shown on
 the map. The location of and metadata about the tiles are defined either by an
 option dictionary or by an external file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.
 A tile source is added to an `MGLStyle` object along with one or more
 `MGLRasterStyleLayer` or `MGLVectorStyleLayer` objects. Use a style layer to
 control the appearance of content supplied by the tile source.
 
 A tile source is also known as a tile set. To learn about the structure of a
 Mapbox-hosted tile set, view it in
 <a href="https://www.mapbox.com/studio/tilesets/">Mapbox Studio’s Tilesets editor</a>.

 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of `MGLRasterSource` and
 `MGLVectorSource`.
 */
MGL_EXPORT
@interface MGLTileSource : MGLSource

#pragma mark Initializing a Source

- (instancetype)init __attribute__((unavailable("Use -initWithIdentifier:configurationURL: or -initWithIdentifier:tileURLTemplates:options: instead.")));
- (instancetype)initWithIdentifier:(NSString *)identifier __attribute__((unavailable("Use -initWithIdentifier:configurationURL: or -initWithIdentifier:tileURLTemplates:options: instead.")));

/**
 Returns a tile source initialized with an identifier and configuration URL.

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
 @return An initialized tile source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL;

/**
 Returns a tile source initialized an identifier, tile URL templates, and
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
- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options;

#pragma mark Accessing a Source’s Content

/**
 The URL to the TileJSON configuration file that specifies the contents of the
 source.

 If the receiver was initialized using
 `-initWithIdentifier:tileURLTemplates:options`, this property is set to `nil`.
 */
@property (nonatomic, copy, nullable, readonly) NSURL *configurationURL;

#pragma mark Accessing Attribution Strings

/**
 An array of `MGLAttributionInfo` objects that define the attribution
 statements to be displayed when the map is shown to the user.

 By default, this array is empty. If the source is initialized with a
 configuration URL, this array is also empty until the configuration JSON file
 is loaded.
 */
@property (nonatomic, copy, readonly) NS_ARRAY_OF(MGLAttributionInfo *) *attributionInfos;

@end

NS_ASSUME_NONNULL_END
