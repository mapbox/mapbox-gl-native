#import "MGLSource.h"
#import "MGLGeoJSONSource.h"
#import "MGLGeoJSONSourceBase.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

/**
 Data source for `MGLCustomVectorSource`.
 */
@protocol MGLCustomVectorSourceDataSource <NSObject>

/** 
 Fetch data for a tile
 @param zoom
 @param y
 @param x
 @param completionHandler A block to call with the data that has been fetched for the tile.
 */
- (void)getTileForZoomLevel:(NSUInteger)zoomLevel x:(NSUInteger)x y:(NSUInteger)y completionHandler:( void (^)(NSArray<id <MGLFeature>>*) )completionHandler;

@end

/**
 A source for vector data that is fetched 1 tile at a time. Useful for sources that are
 too large to fit in memory, or are already divided into tiles, but not in Mapbox Vector Tile format.
 */
@interface MGLCustomVectorSource : MGLGeoJSONSourceBase

/**
 Returns a custom vector datasource initialized with an identifier, datasource,  and a
 dictionary of options for the source according to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">style
 specification</a>.

 @param identifier A string that uniquely identifies the source.
 @param dataSource An object that implements the `MGLCustomVectorSourceDataSource` protocol that will be queried for tile data.
 @param options An `NSDictionary` of options for this source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(NSObject<MGLCustomVectorSourceDataSource>*)dataSource options:(NS_DICTIONARY_OF(NSString *, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 Request that the source reloads a tile. Tile will only be reloaded if it is currently on screen.
 */
- (void)setNeedsUpdateAtZoomLevel:(NSUInteger)z x:(NSUInteger)x y:(NSUInteger)y;

/**
 Reload all tiles.
 */
- (void)reloadData;

/**
 An object that implements the `MGLCustomVectorSourceDataSource` protocol that will be queried for tile data.
 */
@property (nonatomic, readonly, copy) NSObject<MGLCustomVectorSourceDataSource> *dataSource;

/**
 A queue that calls to the datasource will be made on.
 */
@property (nonatomic, readonly) NSOperationQueue *requestQueue;

@end

NS_ASSUME_NONNULL_END
