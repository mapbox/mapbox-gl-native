#import "MGLOfflinePack.h"

#include <mbgl/storage/default_file_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLOfflinePack (Private)

@property (nonatomic, nullable) mbgl::OfflineRegion *mbglOfflineRegion;

- (instancetype)initWithMBGLRegion:(mbgl::OfflineRegion *)region;

/**
 Invalidates the pack and ensures that no future progress update can ever
 revalidate it.
 */
- (void)invalidate;

@end

NS_ASSUME_NONNULL_END
