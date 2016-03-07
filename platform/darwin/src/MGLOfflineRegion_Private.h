#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

#include <mbgl/storage/offline.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflineRegion_Private <MGLOfflineRegion>

- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineRegionDefinition &)definition;

- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition;

@end

NS_ASSUME_NONNULL_END
