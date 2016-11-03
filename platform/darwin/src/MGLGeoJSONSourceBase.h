#import "MGLSource.h"


/**
 Options for `MGLGeoJSONSource` objects.
 */
typedef NSString *MGLGeoJSONSourceOption NS_STRING_ENUM;

/**
 An `NSNumber` object containing a Boolean enabling or disabling clustering.
 If the `features` property contains point features, setting this option to
 `YES` clusters the points by radius into groups. The default value is `NO`.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionClustered;

/**
 An `NSNumber` object containing an integer; specifies the radius of each
 cluster if clustering is enabled. A value of 512 produces a radius equal to
 the width of a tile. The default value is 50.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionClusterRadius;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to cluster points if clustering is enabled. Defaults to one zoom level
 less than the value of `MGLGeoJSONSourceOptionMaximumZoomLevel` so that, at the
 maximum zoom level, the features are not clustered.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMaximumZoomLevelForClustering;

/**
 An `NSNumber` object containing an integer; specifies the minimum zoom level at
 which to create vector tiles. The default value is 0.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMinimumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to create vector tiles. A greater value produces greater detail at high
 zoom levels. The default value is 18.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMaximumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the size of the tile
 buffer on each side. A value of 0 produces no buffer. A value of 512 produces a
 buffer as wide as the tile itself. Larger values produce fewer rendering
 artifacts near tile edges and slower performance. The default value is 128.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionBuffer;

/**
 An `NSNumber` object containing a double; specifies the Douglas-Peucker
 simplification tolerance. A greater value produces simpler geometries and
 improves performance. The default value is 0.375.
 */
extern const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionSimplificationTolerance;


@interface MGLGeoJSONSourceBase : MGLSource


@end
