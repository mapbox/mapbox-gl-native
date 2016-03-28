#import "MGLOfflineStorage_Private.h"

#import "MGLAccountManager_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLOfflinePack_Private.h"
#import "MGLOfflineRegion_Private.h"
#import "MGLTilePyramidOfflineRegion.h"

#include <mbgl/util/string.hpp>

@interface MGLOfflineStorage ()

@property (nonatomic) mbgl::DefaultFileSource *mbglFileSource;

- (instancetype)initWithFileName:(NSString *)fileName NS_DESIGNATED_INITIALIZER;

@end

@implementation MGLOfflineStorage

+ (instancetype)sharedOfflineStorage {
    static dispatch_once_t onceToken;
    static MGLOfflineStorage *sharedOfflineStorage;
    dispatch_once(&onceToken, ^{
        sharedOfflineStorage = [[self alloc] initWithFileName:@"cache.db"];
    });
    return sharedOfflineStorage;
}

// This method can’t be called -init, because that selector has been marked
// unavailable in MGLOfflineStorage.h.
- (instancetype)initWithFileName:(NSString *)fileName {
    if (self = [super init]) {
        // Place the cache in a location specific to the application, so that
        // packs downloaded by other applications don’t count toward this
        // application’s limits.
        // ~/Library/Application Support/tld.app.bundle.id/cache.db
        NSURL *cacheDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
                                                                          inDomain:NSUserDomainMask
                                                                 appropriateForURL:nil
                                                                            create:YES
                                                                             error:nil];
        cacheDirectoryURL = [cacheDirectoryURL URLByAppendingPathComponent:
                             [NSBundle mainBundle].bundleIdentifier];
        [[NSFileManager defaultManager] createDirectoryAtURL:cacheDirectoryURL
                                 withIntermediateDirectories:YES
                                                  attributes:nil
                                                       error:nil];
        NSURL *cacheURL = [cacheDirectoryURL URLByAppendingPathComponent:fileName];
        NSString *cachePath = cacheURL ? cacheURL.path : @"";
        
        // Avoid backing up the offline cache onto iCloud, because it can be
        // redownloaded. Ideally, we’d even put the ambient cache in Caches, so
        // it can be reclaimed by the system when disk space runs low. But
        // unfortunately it has to live in the same file as offline resources.
        [cacheURL setResourceValue:@YES forKey:NSURLIsExcludedFromBackupKey error:NULL];
        
        // Move the offline cache from v3.2.0-beta.1 to a location that can also
        // be used for ambient caching.
        NSString *legacyCacheFileName = @"offline.db";
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        // ~/Documents/offline.db
        NSArray *legacyPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *legacyCachePath = [legacyPaths.firstObject stringByAppendingPathComponent:legacyCacheFileName];
#elif TARGET_OS_MAC
        // ~/Library/Caches/tld.app.bundle.id/offline.db
        NSURL *legacyCacheDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSCachesDirectory
                                                                                inDomain:NSUserDomainMask
                                                                       appropriateForURL:nil
                                                                                  create:YES
                                                                                   error:nil];
        legacyCacheDirectoryURL = [legacyCacheDirectoryURL URLByAppendingPathComponent:
                                   [NSBundle mainBundle].bundleIdentifier];
        [[NSFileManager defaultManager] createDirectoryAtURL:legacyCacheDirectoryURL
                                 withIntermediateDirectories:YES
                                                  attributes:nil
                                                       error:nil];
        NSURL *legacyCacheURL = [legacyCacheDirectoryURL URLByAppendingPathComponent:legacyCacheFileName];
        NSString *legacyCachePath = legacyCacheURL ? legacyCacheURL.path : @"";
#endif
        if (![[NSFileManager defaultManager] fileExistsAtPath:cachePath]) {
            [[NSFileManager defaultManager] moveItemAtPath:legacyCachePath toPath:cachePath error:NULL];
        }
        
        _mbglFileSource = new mbgl::DefaultFileSource(cachePath.UTF8String, [NSBundle mainBundle].resourceURL.path.UTF8String);
        
        // Observe for changes to the global access token (and find out the current one).
        [[MGLAccountManager sharedManager] addObserver:self
                                            forKeyPath:@"accessToken"
                                               options:(NSKeyValueObservingOptionInitial |
                                                        NSKeyValueObservingOptionNew)
                                               context:NULL];
    }
    return self;
}

- (void)dealloc {
    [[MGLAccountManager sharedManager] removeObserver:self forKeyPath:@"accessToken"];
    
    delete _mbglFileSource;
    _mbglFileSource = nullptr;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NS_DICTIONARY_OF(NSString *, id) *)change context:(__unused void *)context {
    // Synchronize the file source’s access token with the global one in MGLAccountManager.
    if ([keyPath isEqualToString:@"accessToken"] && object == [MGLAccountManager sharedManager]) {
        NSString *accessToken = change[NSKeyValueChangeNewKey];
        if (![accessToken isKindOfClass:[NSNull class]]) {
            self.mbglFileSource->setAccessToken(accessToken.UTF8String);
        }
    }
}

- (void)addPackForRegion:(id <MGLOfflineRegion>)region withContext:(NSData *)context completionHandler:(MGLOfflinePackAdditionCompletionHandler)completion {
    if (![region conformsToProtocol:@protocol(MGLOfflineRegion_Private)]) {
        [NSException raise:@"Unsupported region type" format:
         @"Regions of type %@ are unsupported.", NSStringFromClass([region class])];
        return;
    }
    
    const mbgl::OfflineTilePyramidRegionDefinition regionDefinition = [(id <MGLOfflineRegion_Private>)region offlineRegionDefinition];
    mbgl::OfflineRegionMetadata metadata(context.length);
    [context getBytes:&metadata[0] length:metadata.size()];
    self.mbglFileSource->createOfflineRegion(regionDefinition, metadata, [&, completion](std::exception_ptr exception, mbgl::optional<mbgl::OfflineRegion> mbglOfflineRegion) {
        NSError *error;
        if (exception) {
            NSString *errorDescription = @(mbgl::util::toString(exception).c_str());
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:errorDescription ? @{
                NSLocalizedDescriptionKey: errorDescription,
            } : nil];
        }
        if (completion) {
            MGLOfflinePack *pack = mbglOfflineRegion ? [[MGLOfflinePack alloc] initWithMBGLRegion:new mbgl::OfflineRegion(std::move(*mbglOfflineRegion))] : nil;
            dispatch_async(dispatch_get_main_queue(), [&, completion, error, pack](void) {
                completion(pack, error);
            });
        }
    });
}

- (void)removePack:(MGLOfflinePack *)pack withCompletionHandler:(MGLOfflinePackRemovalCompletionHandler)completion {
    mbgl::OfflineRegion *mbglOfflineRegion = pack.mbglOfflineRegion;
    [pack invalidate];
    self.mbglFileSource->deleteOfflineRegion(std::move(*mbglOfflineRegion), [&, completion](std::exception_ptr exception) {
        NSError *error;
        if (exception) {
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:@{
                NSLocalizedDescriptionKey: @(mbgl::util::toString(exception).c_str()),
            }];
        }
        if (completion) {
            dispatch_async(dispatch_get_main_queue(), [&, completion, error](void) {
                completion(error);
            });
        }
    });
}

- (void)getPacksWithCompletionHandler:(MGLOfflinePackListingCompletionHandler)completion {
    self.mbglFileSource->listOfflineRegions([&, completion](std::exception_ptr exception, mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) {
        NSError *error;
        if (exception) {
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:@{
                NSLocalizedDescriptionKey: @(mbgl::util::toString(exception).c_str()),
            }];
        }
        NSMutableArray *packs;
        if (regions) {
            packs = [NSMutableArray arrayWithCapacity:regions->size()];
            for (mbgl::OfflineRegion &region : *regions) {
                MGLOfflinePack *pack = [[MGLOfflinePack alloc] initWithMBGLRegion:new mbgl::OfflineRegion(std::move(region))];
                [packs addObject:pack];
            }
        }
        if (completion) {
            dispatch_async(dispatch_get_main_queue(), [&, completion, error, packs](void) {
                completion(packs, error);
            });
        }
    });
}

- (void)setMaximumAllowedMapboxTiles:(uint64_t)maximumCount {
    _mbglFileSource->setOfflineMapboxTileCountLimit(maximumCount);
}

@end
