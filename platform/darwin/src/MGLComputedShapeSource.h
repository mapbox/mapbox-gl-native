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
 protocol must implement one, and only one of the methods.
 */
@protocol MGLComputedShapeSourceDataSource <NSObject>

@optional
/**
 Fetch features for a tile. This will not be called on the main queue, it will be called on the caller's requestQueue.
 @param x tile X coordinate
 @param y tile Y coordinate
 @param zoomLevel tile zoom level
 */
- (NSArray<MGLShape <MGLFeature> *>*)featuresInTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel;

/**
 Fetch features for a tile. This will not be called on the main queue, it will be called on the caller's requestQueue.
 @param bounds The bounds to fetch data for
 @param zoomLevel tile zoom level
 */
- (NSArray<MGLShape <MGLFeature> *>*)featuresInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoomLevel;

@end

/**
 A source for vector data that is fetched one tile at a time. Useful for sources that are
 too large to fit in memory, or are already divided into tiles, but not in Mapbox Vector Tile format.
 */
MGL_EXPORT
@interface MGLComputedShapeSource : MGLAbstractShapeSource

/**
 Returns a custom vector data source initialized with an identifier, data source, and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.

 @param identifier A string that uniquely identifies the source.
 @param options An `NSDictionary` of options for this source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Request that the source reloads a region.
 */
- (void)reloadTileInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoomLevel;

/**
 Reload all tiles.
 */
- (void)reloadData;

/**
 An object that implements the `MGLComputedShapeSource` protocol that will be queried for tile data.
 */
@property (nonatomic, weak, nullable) id<MGLComputedShapeSourceDataSource> dataSource;

/**
 A queue that calls to the data source will be made on.
 */
@property (nonatomic, readonly) NSOperationQueue *requestQueue;

@end

NS_ASSUME_NONNULL_END
