#import "MGLOfflineStorage_Private.h"

#import "MGLFoundation_Private.h"
#import "MGLAccountManager_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLOfflinePack_Private.h"
#import "MGLOfflineRegion_Private.h"
#import "MGLTilePyramidOfflineRegion.h"
#import "MGLShapeOfflineRegion.h"
#import "NSBundle+MGLAdditions.h"
#import "NSValue+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLLoggingConfiguration_Private.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MMEConstants.h"
#import "MGLMapboxEvents.h"
#endif

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>

#include <memory>

static NSString * const MGLOfflineStorageFileName = @"cache.db";
static NSString * const MGLOfflineStorageFileName3_2_0_beta_1 = @"offline.db";

const NSNotificationName MGLOfflinePackProgressChangedNotification = @"MGLOfflinePackProgressChanged";
const NSNotificationName MGLOfflinePackErrorNotification = @"MGLOfflinePackError";
const NSNotificationName MGLOfflinePackMaximumMapboxTilesReachedNotification = @"MGLOfflinePackMaximumMapboxTilesReached";

const MGLOfflinePackUserInfoKey MGLOfflinePackUserInfoKeyState = @"State";
const MGLOfflinePackUserInfoKey MGLOfflinePackUserInfoKeyProgress = @"Progress";
const MGLOfflinePackUserInfoKey MGLOfflinePackUserInfoKeyError = @"Error";
const MGLOfflinePackUserInfoKey MGLOfflinePackUserInfoKeyMaximumCount = @"MaximumCount";

const MGLExceptionName MGLUnsupportedRegionTypeException = @"MGLUnsupportedRegionTypeException";

@interface MGLOfflineStorage ()

@property (nonatomic, strong, readwrite) NSMutableArray<MGLOfflinePack *> *packs;
@property (nonatomic) std::shared_ptr<mbgl::DefaultFileSource> mbglFileSource;
@property (nonatomic) std::string mbglCachePath;
@property (nonatomic, getter=isPaused) BOOL paused;

@end

@implementation MGLOfflineStorage {
    std::unique_ptr<mbgl::Actor<mbgl::ResourceTransform>> _mbglResourceTransform;
}

+ (instancetype)sharedOfflineStorage {
    static dispatch_once_t onceToken;
    static MGLOfflineStorage *sharedOfflineStorage;
    dispatch_once(&onceToken, ^{
        sharedOfflineStorage = [[self alloc] init];
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        [[NSNotificationCenter defaultCenter] addObserver:sharedOfflineStorage selector:@selector(unpauseFileSource:) name:UIApplicationWillEnterForegroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:sharedOfflineStorage selector:@selector(pauseFileSource:) name:UIApplicationDidEnterBackgroundNotification object:nil];
#endif
        [sharedOfflineStorage reloadPacks];
    });

    return sharedOfflineStorage;
}

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
- (void)pauseFileSource:(__unused NSNotification *)notification {
    if (self.isPaused) {
        return;
    }
    _mbglFileSource->pause();
    self.paused = YES;
}

- (void)unpauseFileSource:(__unused NSNotification *)notification {
    if (!self.isPaused) {
        return;
    }
    _mbglFileSource->resume();
    self.paused = NO;
}
#endif

- (void)setDelegate:(id<MGLOfflineStorageDelegate>)newValue {
    MGLLogDebug(@"Setting delegate: %@", newValue);
    _delegate = newValue;
    if ([self.delegate respondsToSelector:@selector(offlineStorage:URLForResourceOfKind:withURL:)]) {
        _mbglResourceTransform = std::make_unique<mbgl::Actor<mbgl::ResourceTransform>>(*mbgl::Scheduler::GetCurrent(), [offlineStorage = self](auto kind_, const std::string&& url_) -> std::string {
            NSURL* url =
            [NSURL URLWithString:[[NSString alloc] initWithBytes:url_.data()
                                                          length:url_.length()
                                                        encoding:NSUTF8StringEncoding]];
            MGLResourceKind kind = MGLResourceKindUnknown;
            switch (kind_) {
                case mbgl::Resource::Kind::Tile:
                    kind = MGLResourceKindTile;
                    break;
                case mbgl::Resource::Kind::Glyphs:
                    kind = MGLResourceKindGlyphs;
                    break;
                case mbgl::Resource::Kind::Style:
                    kind = MGLResourceKindStyle;
                    break;
                case mbgl::Resource::Kind::Source:
                    kind = MGLResourceKindSource;
                    break;
                case mbgl::Resource::Kind::SpriteImage:
                    kind = MGLResourceKindSpriteImage;
                    break;
                case mbgl::Resource::Kind::SpriteJSON:
                    kind = MGLResourceKindSpriteJSON;
                    break;
                case mbgl::Resource::Kind::Image:
                    kind = MGLResourceKindImage;
                    break;
                case mbgl::Resource::Kind::Unknown:
                    kind = MGLResourceKindUnknown;
                    break;

            }
            url = [offlineStorage.delegate offlineStorage:offlineStorage
                                     URLForResourceOfKind:kind
                                                  withURL:url];
            return url.absoluteString.UTF8String;
        });

        _mbglFileSource->setResourceTransform(_mbglResourceTransform->self());
    } else {
        _mbglResourceTransform.reset();
        _mbglFileSource->setResourceTransform({});
    }
}

/**
 Returns the file URL to the offline cache, with the option to omit the private
 subdirectory for legacy (v3.2.0 - v3.2.3) migration purposes.

 The cache is located in a directory specific to the application, so that packs
 downloaded by other applications don’t count toward this application’s limits.

 The cache is located at:
 ~/Library/Application Support/tld.app.bundle.id/.mapbox/cache.db

 The subdirectory-less cache was located at:
 ~/Library/Application Support/tld.app.bundle.id/cache.db
 */
+ (NSURL *)cacheURLIncludingSubdirectory:(BOOL)useSubdirectory {
    NSURL *cacheDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
                                                                      inDomain:NSUserDomainMask
                                                             appropriateForURL:nil
                                                                        create:YES
                                                                         error:nil];
    NSString *bundleIdentifier = [NSBundle mgl_applicationBundleIdentifier];
    if (!bundleIdentifier) {
        // There’s no main bundle identifier when running in a unit test bundle.
        bundleIdentifier = [[NSUUID UUID] UUIDString];
    }
    cacheDirectoryURL = [cacheDirectoryURL URLByAppendingPathComponent:bundleIdentifier];
    if (useSubdirectory) {
        cacheDirectoryURL = [cacheDirectoryURL URLByAppendingPathComponent:@".mapbox"];
    }
    [[NSFileManager defaultManager] createDirectoryAtURL:cacheDirectoryURL
                             withIntermediateDirectories:YES
                                              attributes:nil
                                                   error:nil];
    if (useSubdirectory) {
        // Avoid backing up the offline cache onto iCloud, because it can be
        // redownloaded. Ideally, we’d even put the ambient cache in Caches, so
        // it can be reclaimed by the system when disk space runs low. But
        // unfortunately it has to live in the same file as offline resources.
        [cacheDirectoryURL setResourceValue:@YES forKey:NSURLIsExcludedFromBackupKey error:NULL];
    }
    return [cacheDirectoryURL URLByAppendingPathComponent:MGLOfflineStorageFileName];
}

/**
 Returns the absolute path to the location where v3.2.0-beta.1 placed the
 offline cache.
 */
+ (NSString *)legacyCachePath {
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    // ~/Documents/offline.db
    NSArray *legacyPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *legacyCachePath = [legacyPaths.firstObject stringByAppendingPathComponent:MGLOfflineStorageFileName3_2_0_beta_1];
#elif TARGET_OS_MAC
    // ~/Library/Caches/tld.app.bundle.id/offline.db
    NSString *bundleIdentifier = [NSBundle mgl_applicationBundleIdentifier];
    NSURL *legacyCacheDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSCachesDirectory
                                                                            inDomain:NSUserDomainMask
                                                                   appropriateForURL:nil
                                                                              create:NO
                                                                               error:nil];
    legacyCacheDirectoryURL = [legacyCacheDirectoryURL URLByAppendingPathComponent:bundleIdentifier];
    NSURL *legacyCacheURL = [legacyCacheDirectoryURL URLByAppendingPathComponent:MGLOfflineStorageFileName3_2_0_beta_1];
    NSString *legacyCachePath = legacyCacheURL ? legacyCacheURL.path : @"";
#endif
    return legacyCachePath;
}

- (instancetype)init {
    MGLInitializeRunLoop();

    if (self = [super init]) {
        NSURL *cacheURL = [[self class] cacheURLIncludingSubdirectory:YES];
        NSString *cachePath = cacheURL.path ?: @"";

        // Move the offline cache from v3.2.0-beta.1 to a location that can also
        // be used for ambient caching.
        if (![[NSFileManager defaultManager] fileExistsAtPath:cachePath]) {
            NSString *legacyCachePath = [[self class] legacyCachePath];
            [[NSFileManager defaultManager] moveItemAtPath:legacyCachePath toPath:cachePath error:NULL];
        }

        // Move the offline file cache from v3.2.x path to a subdirectory that
        // can be reliably excluded from backups.
        if (![[NSFileManager defaultManager] fileExistsAtPath:cachePath]) {
            NSURL *subdirectorylessCacheURL = [[self class] cacheURLIncludingSubdirectory:NO];
            [[NSFileManager defaultManager] moveItemAtPath:subdirectorylessCacheURL.path toPath:cachePath error:NULL];
        }

        _mbglCachePath = cachePath.UTF8String;
        mbgl::ResourceOptions options;
        options.withCachePath(_mbglCachePath)
               .withAssetPath([NSBundle mainBundle].resourceURL.path.UTF8String);
        _mbglFileSource = std::static_pointer_cast<mbgl::DefaultFileSource>(mbgl::FileSource::getSharedFileSource(options));

        // Observe for changes to the API base URL (and find out the current one).
        [[MGLAccountManager sharedManager] addObserver:self
                                            forKeyPath:@"apiBaseURL"
                                               options:(NSKeyValueObservingOptionInitial |
                                                              NSKeyValueObservingOptionNew)
                                               context:NULL];

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
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [[MGLAccountManager sharedManager] removeObserver:self forKeyPath:@"apiBaseURL"];
    [[MGLAccountManager sharedManager] removeObserver:self forKeyPath:@"accessToken"];

    for (MGLOfflinePack *pack in self.packs) {
        [pack invalidate];
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSString *, id> *)change context:(void *)context {
    // Synchronize the file source’s access token with the global one in MGLAccountManager.
    if ([keyPath isEqualToString:@"accessToken"] && object == [MGLAccountManager sharedManager]) {
        NSString *accessToken = change[NSKeyValueChangeNewKey];
        if (![accessToken isKindOfClass:[NSNull class]]) {
            _mbglFileSource->setAccessToken(accessToken.UTF8String);
        }
    } else if ([keyPath isEqualToString:@"apiBaseURL"] && object == [MGLAccountManager sharedManager]) {
        NSURL *apiBaseURL = change[NSKeyValueChangeNewKey];
        if ([apiBaseURL isKindOfClass:[NSNull class]]) {
            _mbglFileSource->setAPIBaseURL(mbgl::util::API_BASE_URL);
        } else {
            _mbglFileSource->setAPIBaseURL(apiBaseURL.absoluteString.UTF8String);
        }
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

#pragma mark Offline merge methods

- (void)addContentsOfFile:(NSString *)filePath withCompletionHandler:(MGLBatchedOfflinePackAdditionCompletionHandler)completion {
    MGLLogDebug(@"Adding contentsOfFile: %@ completionHandler: %@", filePath, completion);
    NSURL *fileURL = [NSURL fileURLWithPath:filePath];
    
    [self addContentsOfURL:fileURL withCompletionHandler:completion];

}

- (void)addContentsOfURL:(NSURL *)fileURL withCompletionHandler:(MGLBatchedOfflinePackAdditionCompletionHandler)completion {
    MGLLogDebug(@"Adding contentsOfURL: %@ completionHandler: %@", fileURL, completion);
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    if (!fileURL.isFileURL) {
        [NSException raise:NSInvalidArgumentException format:@"%@ must be a valid file path", fileURL.absoluteString];
    }
    if (![fileManager isWritableFileAtPath:fileURL.path]) {
        [NSException raise:NSInvalidArgumentException format:@"The file path: %@ must be writable", fileURL.absoluteString];
    }
    
    __weak MGLOfflineStorage *weakSelf = self;
    [self _addContentsOfFile:fileURL.path withCompletionHandler:^(NSArray<MGLOfflinePack *> * _Nullable packs, NSError * _Nullable error) {
        if (packs) {
            NSMutableDictionary *packsByIdentifier = [NSMutableDictionary dictionary];
            
            MGLOfflineStorage *strongSelf = weakSelf;
            for (MGLOfflinePack *pack in packs) {
                [packsByIdentifier setObject:pack forKey:@(pack.mbglOfflineRegion->getID())];
            }
            
            id mutablePacks = [strongSelf mutableArrayValueForKey:@"packs"];
            NSMutableIndexSet *replaceIndexSet = [NSMutableIndexSet indexSet];
            NSMutableArray *replacePacksArray = [NSMutableArray array];
            [strongSelf.packs enumerateObjectsUsingBlock:^(MGLOfflinePack * _Nonnull pack, NSUInteger idx, BOOL * _Nonnull stop) {
                MGLOfflinePack *newPack = packsByIdentifier[@(pack.mbglOfflineRegion->getID())];
                if (newPack) {
                    MGLOfflinePack *previousPack = [mutablePacks objectAtIndex:idx];
                    [previousPack invalidate];
                    [replaceIndexSet addIndex:idx];
                    [replacePacksArray addObject:[packsByIdentifier objectForKey:@(newPack.mbglOfflineRegion->getID())]];
                    [packsByIdentifier removeObjectForKey:@(newPack.mbglOfflineRegion->getID())];
                }

            }];
            
            if (replaceIndexSet.count > 0) {
                [mutablePacks replaceObjectsAtIndexes:replaceIndexSet withObjects:replacePacksArray];
            }
            
            [mutablePacks addObjectsFromArray:packsByIdentifier.allValues];
        }
        if (completion) {
            completion(fileURL, packs, error);
        }
    }];
}

- (void)_addContentsOfFile:(NSString *)filePath withCompletionHandler:(void (^)(NSArray<MGLOfflinePack *> * _Nullable packs, NSError * _Nullable error))completion {
    _mbglFileSource->mergeOfflineRegions(std::string(static_cast<const char *>([filePath UTF8String])), [&, completion, filePath](mbgl::expected<mbgl::OfflineRegions, std::exception_ptr> result) {
        NSError *error;
        NSMutableArray *packs;
        if (!result) {
            NSString *description = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"ADD_FILE_CONTENTS_FAILED_DESC", @"Foundation", nil, @"Unable to add offline packs from the file at %@.", @"User-friendly error description"), filePath];
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:@{
                                                                               NSLocalizedDescriptionKey: description,
                                                                               NSLocalizedFailureReasonErrorKey: @(mbgl::util::toString(result.error()).c_str())
                                                                               }];
        } else {
            auto& regions = result.value();
            packs = [NSMutableArray arrayWithCapacity:regions.size()];
            for (auto &region : regions) {
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

#pragma mark Pack management methods

- (void)addPackForRegion:(id <MGLOfflineRegion>)region withContext:(NSData *)context completionHandler:(MGLOfflinePackAdditionCompletionHandler)completion {
    MGLLogDebug(@"Adding packForRegion: %@ contextLength: %lu completionHandler: %@", region, (unsigned long)context.length, completion);
    __weak MGLOfflineStorage *weakSelf = self;
    [self _addPackForRegion:region withContext:context completionHandler:^(MGLOfflinePack * _Nullable pack, NSError * _Nullable error) {
        pack.state = MGLOfflinePackStateInactive;
        MGLOfflineStorage *strongSelf = weakSelf;
        [[strongSelf mutableArrayValueForKey:@"packs"] addObject:pack];
        if (completion) {
            completion(pack, error);
        }
            
        #if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
            NSMutableDictionary *offlineDownloadStartEventAttributes = [NSMutableDictionary dictionaryWithObject:MMEventTypeOfflineDownloadStart forKey:MMEEventKeyEvent];
        
            if ([region conformsToProtocol:@protocol(MGLOfflineRegion_Private)]) {
                NSDictionary *regionAttributes = ((id<MGLOfflineRegion_Private>)region).offlineStartEventAttributes;
                [offlineDownloadStartEventAttributes addEntriesFromDictionary:regionAttributes];
            }
        
            [MGLMapboxEvents pushEvent:MMEventTypeOfflineDownloadStart withAttributes:offlineDownloadStartEventAttributes];
        #endif
    }];
}

- (void)_addPackForRegion:(id <MGLOfflineRegion>)region withContext:(NSData *)context completionHandler:(MGLOfflinePackAdditionCompletionHandler)completion {
    if (![region conformsToProtocol:@protocol(MGLOfflineRegion_Private)]) {
        [NSException raise:MGLUnsupportedRegionTypeException
                    format:@"Regions of type %@ are unsupported.", NSStringFromClass([region class])];
        return;
    }

    const mbgl::OfflineRegionDefinition regionDefinition = [(id <MGLOfflineRegion_Private>)region offlineRegionDefinition];
    mbgl::OfflineRegionMetadata metadata(context.length);
    [context getBytes:&metadata[0] length:metadata.size()];
    _mbglFileSource->createOfflineRegion(regionDefinition, metadata, [&, completion](mbgl::expected<mbgl::OfflineRegion, std::exception_ptr> mbglOfflineRegion) {
        NSError *error;
        if (!mbglOfflineRegion) {
            NSString *errorDescription = @(mbgl::util::toString(mbglOfflineRegion.error()).c_str());
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:errorDescription ? @{
                NSLocalizedDescriptionKey: errorDescription,
            } : nil];
        }
        if (completion) {
            MGLOfflinePack *pack = mbglOfflineRegion ? [[MGLOfflinePack alloc] initWithMBGLRegion:new mbgl::OfflineRegion(std::move(mbglOfflineRegion.value()))] : nil;
            dispatch_async(dispatch_get_main_queue(), [&, completion, error, pack](void) {
                completion(pack, error);
            });
        }
    });
}

- (void)removePack:(MGLOfflinePack *)pack withCompletionHandler:(MGLOfflinePackRemovalCompletionHandler)completion {
    MGLLogDebug(@"Removing pack: %@ completionHandler: %@", pack, completion);
    [[self mutableArrayValueForKey:@"packs"] removeObject:pack];
    [self _removePack:pack withCompletionHandler:^(NSError * _Nullable error) {
        if (completion) {
            completion(error);
        }
    }];
}

- (void)_removePack:(MGLOfflinePack *)pack withCompletionHandler:(MGLOfflinePackRemovalCompletionHandler)completion {
    mbgl::OfflineRegion *mbglOfflineRegion = pack.mbglOfflineRegion;
    [pack invalidate];
    if (!mbglOfflineRegion) {
        completion(nil);
        return;
    }

    _mbglFileSource->deleteOfflineRegion(std::move(*mbglOfflineRegion), [&, completion](std::exception_ptr exception) {
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

- (void)reloadPacks {
    MGLLogInfo(@"Reloading packs.");
    [self getPacksWithCompletionHandler:^(NSArray<MGLOfflinePack *> *packs, __unused NSError * _Nullable error) {
        for (MGLOfflinePack *pack in self.packs) {
            [pack invalidate];
        }
        self.packs = [packs mutableCopy];
    }];
}

- (void)getPacksWithCompletionHandler:(void (^)(NSArray<MGLOfflinePack *> *packs, NSError * _Nullable error))completion {
    _mbglFileSource->listOfflineRegions([&, completion](mbgl::expected<mbgl::OfflineRegions, std::exception_ptr> result) {
        NSError *error;
        NSMutableArray *packs;
        if (!result) {
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:@{
                NSLocalizedDescriptionKey: @(mbgl::util::toString(result.error()).c_str()),
            }];
        } else {
            auto& regions = result.value();
            packs = [NSMutableArray arrayWithCapacity:regions.size()];
            for (auto &region : regions) {
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
    MGLLogDebug(@"Setting maximumAllowedMapboxTiles: %lu", (unsigned long)maximumCount);
    _mbglFileSource->setOfflineMapboxTileCountLimit(maximumCount);
}

#pragma mark - Ambient Cache management


void maxCacheSizeCallback(std::exception_ptr result) {
    
    
    
    //    if ( ! (cacheSize > 0) ) { /* give a warning*/ } ;
   
    //
        // ERROR: Too few arguments to function call, expected 2,
}

- (void)setMaximumAmbientCacheSize:(NSInteger)cacheSize withCallback:(void (^)(NSError *))callback {

    
    _mbglFileSource->setMaximumAmbientCacheSize(cacheSize, maxCacheSizeCallback);

    }

- (void)invalidateAmbientCache {
    _mbglFileSource->invalidateAmbientCache(nil);
    // Do something with the std::exception_ptr here
}

- (void)clearAmbientCache {
    
}
#pragma mark -

- (unsigned long long)countOfBytesCompleted {
    NSURL *cacheURL = [[self class] cacheURLIncludingSubdirectory:YES];
    NSString *cachePath = cacheURL.path;
    if (!cachePath) {
        return 0;
    }

    NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:cachePath error:NULL];
    return attributes.fileSize;
}

- (void)preloadData:(NSData *)data forURL:(NSURL *)url modificationDate:(nullable NSDate *)modified expirationDate:(nullable NSDate *)expires eTag:(nullable NSString *)eTag mustRevalidate:(BOOL)mustRevalidate {
    mbgl::Resource resource(mbgl::Resource::Kind::Unknown, url.absoluteString.UTF8String);
    mbgl::Response response;
    response.data = std::make_shared<std::string>(static_cast<const char*>(data.bytes), data.length);
    response.mustRevalidate = mustRevalidate;
    
    if (eTag) {
        response.etag = std::string(eTag.UTF8String);
    }
    
    if (modified) {
        response.modified = mbgl::Timestamp() + std::chrono::duration_cast<mbgl::Seconds>(MGLDurationFromTimeInterval(modified.timeIntervalSince1970));
    }
    
    if (expires) {
        response.expires = mbgl::Timestamp() + std::chrono::duration_cast<mbgl::Seconds>(MGLDurationFromTimeInterval(expires.timeIntervalSince1970));
    }
    
    _mbglFileSource->put(resource, response);
}

- (void)putResourceWithUrl:(NSURL *)url data:(NSData *)data modified:(nullable NSDate *)modified expires:(nullable NSDate *)expires etag:(nullable NSString *)etag mustRevalidate:(BOOL)mustRevalidate {
    [self preloadData:data forURL:url modificationDate:modified expirationDate:expires eTag:etag mustRevalidate:mustRevalidate];
}

@end
