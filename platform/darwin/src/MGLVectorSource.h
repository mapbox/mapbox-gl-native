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

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

#pragma mark Accessing a Source’s Content

/**
 Returns an array of map features loaded by this source, restricted to the
 given source layers and filtered by the given predicate.

 Each object in the returned array represents a feature for the
 current style and provides access to attributes specified by the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">source</a>.

 Features come from tiled vector data that is converted to tiles internally,
 so feature geometries are clipped at tile boundaries and features
 may appear duplicated across tiles.

 @param sourceLayerIdentifiers The source layers to include in the query. Only the
    features contained in these source layers are included in the returned array. At
    least one source layer is required.
 @param predicate A predicate to filter the returned features.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)featuresInSourceLayersWithIdentifiers:(NS_SET_OF(NSString *) *)sourceLayerIdentifiers predicate:(nullable NSPredicate *)predicate NS_SWIFT_NAME(features(sourceLayerIdentifiers:predicate:));

@end

NS_ASSUME_NONNULL_END
