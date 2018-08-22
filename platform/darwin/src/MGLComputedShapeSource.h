#import "MGLFoundation.h"
#import "MGLGeometry.h"
#import "MGLTypes.h"
#import "MGLShapeSource.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

/**
 An `NSNumber` object containing a Boolean value; specifies whether the shape of
 an `MGLComputedShapeSource` should be wrapped to accomodate coordinates with
 longitudes beyond −180 and 180. The default value is `NO`.
 
 Setting this option to `YES` affects rendering performance.
 
 This option is used with the `MGLComputedShapeSource` class; it is ignored when
 creating an `MGLShapeSource` object.
 */
FOUNDATION_EXTERN MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionWrapsCoordinates;

/**
 An `NSNumber` object containing a Boolean value; specifies whether the shape of
 an `MGLComputedShapeSource` should be clipped at the edge of each tile. The
 default value is `NO`.
 
 Setting this option to `YES` affects rendering performance. Use this option to
 clip `MGLPolyline`s and `MGLPolygon`s at tile boundaries without artifacts.
 
 This option is used with the `MGLComputedShapeSource` class; it is ignored when
 creating an `MGLShapeSource` object.
 */
FOUNDATION_EXTERN MGL_EXPORT const MGLShapeSourceOption MGLShapeSourceOptionClipsCoordinates;

FOUNDATION_EXTERN MGL_EXPORT MGLExceptionName const MGLInvalidDatasourceException;

/**
 Data source for `MGLComputedShapeSource`. This protocol defines two optional methods for fetching
 data, one based on tile coordinates, and one based on a bounding box. Classes that implement this
 protocol must implement one, and only one of the methods. Methods on this protocol will not be
 called on main thread, they will be called on the caller's `requestQueue`.
 */
@protocol MGLComputedShapeSourceDataSource <NSObject>

@optional
/**
 Fetch features for a tile. This method will not be invoked on the main queue, it
 will be invoked on the caller's `requestQueue`.
 @param x Tile X coordinate.
 @param y Tile Y coordinate.
 @param zoomLevel Tile zoom level.
 */
- (NSArray<MGLShape <MGLFeature> *>*)featuresInTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel;

/**
 Fetch features for a tile. This method will not be invoked on the main queue, it
 will be invoked on the caller's `requestQueue`.
 @param bounds The bounds to fetch data for.
 @param zoomLevel Tile zoom level.
 */
- (NSArray<MGLShape <MGLFeature> *>*)featuresInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoomLevel;

@end

/**
 `MGLComputedShapeSource` is a map content source that supplies vector shapes,
 one tile at a time, to be shown on the map on demand. You implement a class
 conforming to the `MGLComputedShapeSourceDataSource` protocol that returns
 instances of `MGLShape` or `MGLFeature`, then add a computed shape source to an
 `MGLStyle` object along with an `MGLVectorStyleLayer` object. The vector style
 layer defines the appearance of any content supplied by the computed shape
 source.
 
 `MGLComputedShapeSource` is similar to `MGLShapeSource` but is optimized for
 data sets that change dynamically or are too large to fit completely in memory.
 It is also useful for data that is divided into tiles in a format other than
 <a href="https://www.mapbox.com/vector-tiles/">Mapbox Vector Tiles</a>. For
 <a href="http://geojson.org/">GeoJSON</a> data, use the `MGLShapeSource` class.
 For static tiles or Mapbox Vector Tiles, use the `MGLVectorTileSource` class.
 
 You can add and remove sources dynamically using methods such as
 `-[MGLStyle addSource:]` and `-[MGLStyle sourceWithIdentifier:]`. This class
 cannot be represented in a style JSON file; you must add it ot the style at
 runtime.
 */
MGL_EXPORT
@interface MGLComputedShapeSource : MGLSource

/**
 Returns a custom shape data source initialized with an identifier, and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 This class supports the following options:
 `MGLShapeSourceOptionMinimumZoomLevel`, `MGLShapeSourceOptionMaximumZoomLevel`,
 `MGLShapeSourceOptionBuffer`,
 `MGLShapeSourceOptionSimplificationTolerance`,
 `MGLShapeSourceOptionWrapsCoordinates`, and
 `MGLShapeSourceOptionClipsCoordinates`. Shapes provided by a computed
 shape source cannot be clustered.

 @param identifier A string that uniquely identifies the source.
 @param options An `NSDictionary` of options for this source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier options:(nullable NSDictionary<MGLShapeSourceOption, id> *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a custom shape data source initialized with an identifier, data source, and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.
 
 This class supports the following options:
 `MGLShapeSourceOptionMinimumZoomLevel`, `MGLShapeSourceOptionMaximumZoomLevel`,
 `MGLShapeSourceOptionBuffer`,
 `MGLShapeSourceOptionSimplificationTolerance`,
 `MGLShapeSourceOptionWrapsCoordinates`, and
 `MGLShapeSourceOptionClipsCoordinates`. Shapes provided by a computed shape
 source cannot be clustered.

 @param identifier A string that uniquely identifies the source.
 @param options An `NSDictionary` of options for this source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(id<MGLComputedShapeSourceDataSource>)dataSource options:(nullable NSDictionary<MGLShapeSourceOption, id> *)options;

/**
 Invalidates all the features and properties intersecting with or contained in
 the specified bounds. New fetch requests will immediately be invoked on the
 `MGLComputedShapeSourceDataSource`.
 @param bounds  Coordinate bounds to invalidate.
 */
- (void) invalidateBounds:(MGLCoordinateBounds)bounds;

/**
 Invalidates all the feautres and properties of a given tile. A new fetch request
 will immediately be invoked on the `MGLComputedShapeSourceDataSource`.
 @param x Tile X coordinate.
 @param y Tile Y coordinate.
 @param zoomLevel Tile zoom level.
 */
- (void) invalidateTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel;

/**
 Set a new set of features for a tile. This method can be invkoed from background threads.
 For best performance, use this method only to update tiles that have already been requested
 through `MGLComputedShapeSourceDataSource.`
 @param features  Features for the tile.
 @param x         Tile X coordinate.
 @param y         Tile Y coordinate.
 @param zoomLevel Tile zoom level.
 */
- (void) setFeatures:(NSArray<MGLShape <MGLFeature> *>*)features inTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel;

/**
 An object that implements the `MGLComputedShapeSourceDataSource` protocol that will be queried for tile data.
 */
@property (nonatomic, weak, nullable) id<MGLComputedShapeSourceDataSource> dataSource;

/**
 A queue that calls to the data source will be made on.
 */
@property (nonatomic, readonly) NSOperationQueue *requestQueue;

@end

NS_ASSUME_NONNULL_END
