#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

#include <mbgl/storage/offline.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflineRegion_Private <MGLOfflineRegion>

/**
 Creates and returns a C++ offline region definition corresponding to the
 receiver.
 */
- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition;

@end

NS_ASSUME_NONNULL_END
