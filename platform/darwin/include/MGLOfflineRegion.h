#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An object conforming to the `MGLOfflineRegion` protocol determines which
 resources are required by an `MGLOfflineTask` object. At present, only
 instances of `MGLTilePyramidOfflineRegion` may be used as `MGLOfflineTask`
 regions, but additional conforming implementations may be added in the future.
 */
@protocol MGLOfflineRegion <NSObject>

@end

NS_ASSUME_NONNULL_END
