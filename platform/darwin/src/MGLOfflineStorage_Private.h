#import "MGLOfflineStorage.h"

#import "MGLOfflinePack.h"

#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/database_file_source.hpp>

#include <memory>

NS_ASSUME_NONNULL_BEGIN

@interface MGLOfflineStorage (Private)

/**
 The shared database file source object owned by the shared offline storage object.
 */
@property (nonatomic) std::shared_ptr<mbgl::DatabaseFileSource> mbglDatabaseFileSource;

/**
 The shared online file source object owned by the shared offline storage object.
 */
@property (nonatomic) std::shared_ptr<mbgl::OnlineFileSource> mbglOnlineFileSource;

/**
 The shared resource loader file source object owned by the shared offline storage object.
 */
@property (nonatomic) std::shared_ptr<mbgl::FileSource> mbglFileSource;

/**
 The shared offline cache path.
 */
@property (nonatomic) std::string mbglCachePath;

@end

NS_ASSUME_NONNULL_END
