#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

#include <mbgl/storage/offline.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflineRegion_Private <MGLOfflineRegion>

/**
 Initializes and returns an offline region backed by the given C++ region
 definition object.

 @param definition A reference to an offline region definition backing the
    offline region.
 */
- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineRegionDefinition &)definition;

/**
 Creates and returns a C++ offline region definition corresponding to the
 receiver.
 */
- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition;

@end

NS_ASSUME_NONNULL_END
