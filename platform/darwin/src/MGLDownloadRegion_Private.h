#import <Foundation/Foundation.h>

#import "MGLDownloadRegion.h"

#include <mbgl/storage/offline.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLDownloadRegion_Private <MGLDownloadRegion>

- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineRegionDefinition &)definition;

- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition;

@end

NS_ASSUME_NONNULL_END
