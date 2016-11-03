#import "MGLSource.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLFeature;


@protocol MGLCustomVectorSourceDataSource <NSObject>

/** 
 Fetch data for a tile
 @param zoom
 @param y
 @param x
 @param callback A block to call with the data that has been fetched for the tile. The data can be in the form of a
 GeoJSON Feature, FeatureCollection, or Geometry.
 */
- (void)getTileForZoom:(NSInteger)zoom x:(NSInteger)x y:(NSInteger)y callback:( void (^)(NSArray<id <MGLFeature>>*) )callback;

@end

@interface MGLCustomVectorSource : MGLSource

/**
 Returns a custom vector datasource initialized with an identifier, datasource, and zoom levels.

 @param identifier A string that uniquely identifies the source.
 @param minimumZoomLevel The minimum zoom level at which the source will display tiles.
 @param maximumZoomLevel The maximum zoom level at which the source will display tiles.
 @param dataSource An object that implements the `MGLCustomVectorSourceDataSource` protocol that will be queried for tile data.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel dataSource:(NSObject<MGLCustomVectorSourceDataSource>*)dataSource NS_DESIGNATED_INITIALIZER;

/**
 An object that implements the `MGLCustomVectorSourceDataSource` protocol that will be queried for tile data.
 */
@property (nonatomic, readonly, copy) NSObject<MGLCustomVectorSourceDataSource> *dataSource;

/**
 An `NSNumber` object containing an integer; specifies the minimum zoom level at
 which the source will display tiles. The value should be in the range of 0 to
 22.
 */
@property (nonatomic, nonnull) NSNumber *minimumZoomLevel;

/**
 An `NSNumber` object containing an integer; specifies the maximum zoom level at
 which to display tiles. The value should be in the range of 0 to 22 and greater
 than `minimumZoomLevel`.
 */
@property (nonatomic, nonnull) NSNumber *maximumZoomLevel;

/**
 An `NSString` object that contains an attribution to be displayed when the map
 is shown to a user. The default value is `nil`.
 */
@property (nonatomic, copy, nullable) NSString *attribution;


@end

NS_ASSUME_NONNULL_END
