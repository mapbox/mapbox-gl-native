#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class MGLTileSource;

/**
 An object conforming to the `MGLOfflineRegion` protocol determines which
 resources are required by an `MGLOfflinePack` object. At present, only
 instances of `MGLTilePyramidOfflineRegion` may be used as `MGLOfflinePack`
 regions, but additional conforming implementations may be added in the future.
 */
@protocol MGLOfflineRegion <NSObject>

/**
 The number of tiles needed to load one of the style’s sources within the region.
 */
-(uint64_t)countTilesForTileSource:(MGLTileSource *)tileSource;

@end

NS_ASSUME_NONNULL_END
