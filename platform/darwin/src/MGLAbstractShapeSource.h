#import "MGLSource.h"

/**
 Options for `MGLShapeSource` objects.
 */
typedef NSString *MGLShapeSourceOption NS_STRING_ENUM;

/**
 An `NSNumber` object containing a Boolean enabling or disabling clustering.
 If the `shape` property contains point shapes, setting this option to
 `YES` clusters the points by radius into groups. The default value is `NO`.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-cluster"><code>cluster</code></a>
 source property in the Mapbox Style Specification.
 
 This option only affects point features within a shape source.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionClustered;

/**
 An `NSNumber` object containing an integer; specifies the radius of each
 cluster if clustering is enabled. A value of 512 produces a radius equal to
 the width of a tile. The default value is 50.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionClusterRadius;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to cluster points if clustering is enabled. Defaults to one zoom level
 less than the value of `MGLShapeSourceOptionMaximumZoomLevel` so that, at the
 maximum zoom level, the shapes are not clustered.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-clusterMaxZoom"><code>clusterMaxZoom</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevelForClustering;

/**
 An `NSNumber` object containing an integer; specifies the minimum zoom level at
 which to create vector tiles. The default value is 0.

 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-minzoom"><code>minzoom</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionMinimumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to create vector tiles. A greater value produces greater detail at high
 zoom levels. The default value is 18.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-maxzoom"><code>maxzoom</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the size of the tile
 buffer on each side. A value of 0 produces no buffer. A value of 512 produces a
 buffer as wide as the tile itself. Larger values produce fewer rendering
 artifacts near tile edges and slower performance. The default value is 128.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-buffer"><code>buffer</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionBuffer;

/**
 An `NSNumber` object containing a double; specifies the Douglas-Peucker
 simplification tolerance. A greater value produces simpler geometries and
 improves performance. The default value is 0.375.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson-tolerance"><code>tolerance</code></a>
 source property in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionSimplificationTolerance;

/**
 An `NSNumber` object containing a Boolean value; specifies whether the shape of
 an `MGLComputedShapeSource` should be wrapped to accomodate coordinates with
 longitudes beyond −180 and 180. The default value is `NO`.
 
 Setting this option to `YES` affects rendering performance.
 
 This option is ignored when creating an instance of a class besides
 `MGLComputedShapeSource`.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionWrapsCoordinates;

/**
 An `NSNumber` object containing a Boolean value; specifies whether the shape of
 an `MGLComputedShapeSource` should be clipped at the edge of each tile. The
 default value is `NO`.
 
 Setting this option to `YES` affects rendering performance. Use this option to
 clip `MGLPolyline`s and `MGLPolygon`s at tile boundaries without artifacts.
 
 This option is ignored when creating an instance of a class besides
 `MGLComputedShapeSource`.
 */
extern MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionClipsCoordinates;

/**
 `MGLAbstractShapeSource` is an abstract base class for map content sources that
 supply vector shapes to be shown on the map. A shape source is added to an
 `MGLStyle` object along with an `MGLVectorStyleLayer` object. The vector style
 layer defines the appearance of any content supplied by the shape source.

 
 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of `MGLShapeSource` or
 `MGLComputedShapeSource`.
 */
MGL_EXPORT
@interface MGLAbstractShapeSource : MGLSource


@end
