#import "MGLFileCache.h"

@interface MGLFileCache ()

@property (nonatomic) MGLFileCache *sharedInstance;
@property (nonatomic) mbgl::SQLiteCache *sharedCache;
@property (nonatomic) NSHashTable *retainers;

@end

@implementation MGLFileCache

const std::string &defaultCacheDatabasePath() {
    static const std::string path = []() -> std::string {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        if ([paths count] == 0) {
            // Disable the cache if we don't have a location to write.
            return "";
        }

        NSString *libraryDirectory = [paths objectAtIndex:0];
        return [[libraryDirectory stringByAppendingPathComponent:@"cache.db"] UTF8String];
    }();
    return path;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _retainers = [NSHashTable weakObjectsHashTable];
    }
    return self;
}

+ (instancetype)sharedInstance {
    static dispatch_once_t onceToken;
    static MGLFileCache *_sharedInstance;
    dispatch_once(&onceToken, ^{
        _sharedInstance = [[self alloc] init];
    });
    return _sharedInstance;
}

- (void)teardown {
    if (self.sharedCache) {
        delete self.sharedCache;
        self.sharedCache = nullptr;
    }
}

- (void)dealloc {
    [self.retainers removeAllObjects];
    [self teardown];
}

+ (mbgl::SQLiteCache *)obtainSharedCacheWithObject:(NSObject *)object {
    return [[MGLFileCache sharedInstance] obtainSharedCacheWithObject:object];
}

- (mbgl::SQLiteCache *)obtainSharedCacheWithObject:(NSObject *)object {
    assert([[NSThread currentThread] isMainThread]);
    if (!self.sharedCache) {
        self.sharedCache = new mbgl::SQLiteCache(defaultCacheDatabasePath());
    }
    [self.retainers addObject:object];
    return self.sharedCache;
}

+ (void)releaseSharedCacheForObject:(NSObject *)object {
    return [[MGLFileCache sharedInstance] releaseSharedCacheForObject:object];
}

- (void)releaseSharedCacheForObject:(NSObject *)object {
    assert([[NSThread currentThread] isMainThread]);
    if ([self.retainers containsObject:object]) {
        [self.retainers removeObject:object];
    }
    if ([self.retainers count] == 0) {
        [self teardown];
    }
}

@end
