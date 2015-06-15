#import "MGLTypes.h"

#include <mbgl/storage/sqlite_cache.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLFileCache : NSObject

+ (mbgl::SQLiteCache *)obtainSharedCacheWithObject:(NSObject *)object;
+ (void)releaseSharedCacheForObject:(NSObject *)object;

@end

NS_ASSUME_NONNULL_END
