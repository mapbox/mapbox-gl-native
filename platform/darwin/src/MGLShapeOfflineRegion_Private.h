#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

#include <mbgl/storage/offline.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLShapeOfflineRegion_Private <MGLOfflineRegion>

/**
 Initializes and returns an offline region backed by the given C++ region
 definition object.

 @param definition A reference to an offline region definition backing the
    offline region.
 */
- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineGeometryRegionDefinition &)definition;

@end

NS_ASSUME_NONNULL_END
