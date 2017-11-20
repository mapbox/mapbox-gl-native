#import "MGLAbstractShapeSource.h"

#import "MGLFoundation.h"
#import "MGLGeometry.h"
#import "MGLTypes.h"
#import "MGLShape.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

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
 A source for vector data that is fetched one tile at a time. Useful for sources that are
 too large to fit in memory, or are already divided into tiles, but not in Mapbox Vector Tile format.
 
 Supported options are `MGLShapeSourceOptionMinimumZoomLevel`, `MGLShapeSourceOptionMaximumZoomLevel`,
  `MGLShapeSourceOptionBuffer`, and `MGLShapeSourceOptionSimplificationTolerance.` This source does
  not support clustering.
 */
MGL_EXPORT
@interface MGLComputedShapeSource : MGLAbstractShapeSource

/**
 Returns a custom shape data source initialized with an identifier, and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.

 @param identifier A string that uniquely identifies the source.
 @param options An `NSDictionary` of options for this source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Returns a custom shape data source initialized with an identifier, data source, and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.

 @param identifier A string that uniquely identifies the source.
 @param options An `NSDictionary` of options for this source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(id<MGLComputedShapeSourceDataSource>)dataSource options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options;

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
