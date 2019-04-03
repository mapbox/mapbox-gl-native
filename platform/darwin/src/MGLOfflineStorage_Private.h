#import "MGLOfflineStorage.h"

#import "MGLOfflinePack.h"

#include <mbgl/storage/default_file_source.hpp>

#include <memory>

NS_ASSUME_NONNULL_BEGIN

@interface MGLOfflineStorage (Private)

/**
 The shared file source object owned by the shared offline storage object.
 */
@property (nonatomic) std::shared_ptr<mbgl::DefaultFileSource> mbglFileSource;

/**
 The shared offline cache path.
 */
@property (nonatomic) std::string mbglCachePath;

@end

NS_ASSUME_NONNULL_END
