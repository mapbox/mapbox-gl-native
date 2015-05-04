#import <Foundation/Foundation.h>

#include <mbgl/storage/sqlite_cache.hpp>

@interface MGLFileCache : NSObject

+ (mbgl::SQLiteCache *)obtainSharedCacheWithObject:(NSObject *)object;
+ (void)releaseSharedCacheForObject:(NSObject *)object;

@end
