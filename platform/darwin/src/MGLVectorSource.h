#import "MGLFeature.h"
#import "MGLFoundation.h"
#import "MGLTileSource.h"

NS_ASSUME_NONNULL_BEGIN

/**
 `MGLVectorSource` is a map content source that supplies tiled vector data in
 <a href="https://www.mapbox.com/vector-tiles/">Mapbox Vector Tile</a> format to
 be shown on the map. The location of and metadata about the tiles are defined
 either by an option dictionary or by an external file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.
 A vector source is added to an `MGLStyle` object along with one or more
 `MGLVectorStyleLayer` objects. A vector style layer defines the appearance of
 any content supplied by the vector source.

 Each
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector"><code>vector</code></a>
 source defined by the style JSON file is represented at runtime by an
 `MGLVectorSource` object that you can use to initialize new style layers. You
 can also add and remove sources dynamically using methods such as
 `-[MGLStyle addSource:]` and `-[MGLStyle sourceWithIdentifier:]`.

 Within each vector tile, each geometric coordinate must lie between
 −1&nbsp;×&nbsp;<var>extent</var> and
 (<var>extent</var>&nbsp;×&nbsp;2)&nbsp;−&nbsp;1, inclusive. Any vector style
 layer initialized with a vector source must have a non-`nil` value in its
 `sourceLayerIdentifier` property.
 
 Commonly used vector sources include
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets</a>,
 <a href="https://www.mapbox.com/vector-tiles/mapbox-terrain/">Mapbox Terrain</a>,
 and
 <a href="https://www.mapbox.com/vector-tiles/mapbox-traffic-v1/">Mapbox Traffic</a>.

 ### Example

 ```swift
 let source = MGLVectorSource(identifier: "pois", tileURLTemplates: ["https://example.com/vector-tiles/{z}/{x}/{y}.mvt"], options: [
     .minimumZoomLevel: 9,
     .maximumZoomLevel: 16,
     .attributionInfos: [
         MGLAttributionInfo(title: NSAttributedString(string: "© Mapbox"), url: URL(string: "http://mapbox.com"))
     ]
 ])
 mapView.style?.addSource(source)
 ```
 */
MGL_EXPORT
@interface MGLVectorSource : MGLTileSource

#pragma mark Initializing a Source

/**
 Returns a vector source initialized with an identifier and configuration URL.

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
 @return An initialized vector source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL NS_DESIGNATED_INITIALIZER;

/**
 Returns a vector source initialized an identifier, tile URL templates, and
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

#pragma mark Accessing a Source’s Content

/**
 Returns an array of map features loaded by this source, restricted to the given
 source layers and filtered by the given predicate.

 Each object in the returned array represents a feature loaded by the source and
 provides access to attributes specified as part of the loaded feature. The
 source loads a feature if the source is added to an `MGLMapView`’s style; that
 style has a layer that uses the source; and the map view has recently scrolled
 to the region containing the feature.

 Features come from tiled vector data that is converted to tiles internally, so
 feature geometries are clipped at tile boundaries and features may appear
 duplicated across tiles. For example, suppose part of a lengthy polyline
 representing a road has recently scrolled into view. The resulting array
 includes those parts of the road that lie within the map tiles that the source
 has loaded, even if the road extends into other tiles. The portion of the road
 within each map tile is included individually.
 
 Returned features may not necessarily be visible to the user at the time they
 are loaded: the style may contain a layer that forces the source’s tiles to
 load but filters out the features in question, preventing them from being
 drawn. To obtain only _visible_ features, use the
 `-[MGLMapView visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:predicate:]`
 or
 `-[MGLMapView visibleFeaturesInRect:inStyleLayersWithIdentifiers:predicate:]`
 method.

 @param sourceLayerIdentifiers The source layers to include in the query. Only
    the features contained in these source layers are included in the returned
    array. This array may not be empty.
 @param predicate A predicate to filter the returned features. Use `nil` to
    include all loaded features.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features loaded by the source that match the predicate.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)featuresInSourceLayersWithIdentifiers:(NS_SET_OF(NSString *) *)sourceLayerIdentifiers predicate:(nullable NSPredicate *)predicate NS_SWIFT_NAME(features(sourceLayerIdentifiers:predicate:));

@end

NS_ASSUME_NONNULL_END
