#import "MGLSource.h"
#import "MGLGeoJSONSource.h"
#import "MGLGeoJSONSourceBase.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;

@protocol MGLCustomVectorSourceDataSource <NSObject>

/** 
 Fetch data for a tile
 @param zoom
 @param y
 @param x
 @param callback A block to call with the data that has been fetched for the tile.
 */
- (void)getTileForZoom:(NSInteger)zoom x:(NSInteger)x y:(NSInteger)y callback:( void (^)(NSArray<id <MGLFeature>>*) )callback;

@end

@interface MGLCustomVectorSource : MGLGeoJSONSourceBase

/**
 Returns a custom vector datasource initialized with an identifier, datasource, and zoom levels.

 @param identifier A string that uniquely identifies the source.
 @param minimumZoomLevel The minimum zoom level at which the source will display tiles.
 @param maximumZoomLevel The maximum zoom level at which the source will display tiles.
 @param dataSource An object that implements the `MGLCustomVectorSourceDataSource` protocol that will be queried for tile data.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(NSObject<MGLCustomVectorSourceDataSource>*)dataSource options:(NS_DICTIONARY_OF(NSString *, id) *)options NS_DESIGNATED_INITIALIZER;

/**
 An object that implements the `MGLCustomVectorSourceDataSource` protocol that will be queried for tile data.
 */
@property (nonatomic, readonly, copy) NSObject<MGLCustomVectorSourceDataSource> *dataSource;


@end

NS_ASSUME_NONNULL_END
