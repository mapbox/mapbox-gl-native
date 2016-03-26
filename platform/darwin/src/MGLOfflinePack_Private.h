#import "MGLOfflinePack.h"

#include <mbgl/storage/default_file_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLOfflinePack (Private)

@property (nonatomic, nullable) mbgl::OfflineRegion *mbglOfflineRegion;

- (instancetype)initWithMBGLRegion:(mbgl::OfflineRegion *)region;

/**
 Request an asynchronous update to the pack’s `state` and `progress` properties.
 
 The state and progress of an inactive or completed pack are computed lazily. If
 you need the state or progress of a pack inside an
 `MGLOfflinePackListingCompletionHandler`, set the `delegate` property then call
 this method.
 */
- (void)requestProgress;

/**
 Invalidates the pack and ensures that no future progress update can ever
 revalidate it.
 */
- (void)invalidate;

@end

NS_ASSUME_NONNULL_END
