#import "MGLDownloadController_Private.h"

#import "MGLAccountManager_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLDownloadable_Private.h"
#import "MGLDownloadRegion_Private.h"
#import "MGLTilePyramidDownloadRegion.h"

#include <mbgl/util/string.hpp>

@interface MGLDownloadController ()

@property (nonatomic) mbgl::DefaultFileSource *mbglFileSource;

- (instancetype)initWithFileName:(NSString *)fileName NS_DESIGNATED_INITIALIZER;

@end

@implementation MGLDownloadController

+ (instancetype)sharedController {
    static dispatch_once_t onceToken;
    static MGLDownloadController *sharedController;
    dispatch_once(&onceToken, ^{
        sharedController = [[self alloc] initWithFileName:@"offline.db"];
    });
    return sharedController;
}

- (instancetype)initWithFileName:(NSString *)fileName {
    if (self = [super init]) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *fileCachePath = [paths.firstObject stringByAppendingPathComponent:fileName];
        _mbglFileSource = new mbgl::DefaultFileSource(fileCachePath.UTF8String, [NSBundle mainBundle].resourceURL.path.UTF8String);
        
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

- (void)addDownloadableForRegion:(id <MGLDownloadRegion>)downloadRegion withContext:(NSData *)context completionHandler:(MGLDownloadableRegistrationCompletionHandler)completion {
    if (![downloadRegion conformsToProtocol:@protocol(MGLDownloadRegion_Private)]) {
        [NSException raise:@"Unsupported region type" format:
         @"Regions of type %@ are unsupported.", NSStringFromClass([downloadRegion class])];
        return;
    }
    
    const mbgl::OfflineTilePyramidRegionDefinition regionDefinition = [(id <MGLDownloadRegion_Private>)downloadRegion offlineRegionDefinition];
    mbgl::OfflineRegionMetadata metadata(context.length);
    [context getBytes:&metadata[0] length:metadata.size()];
    self.mbglFileSource->createOfflineRegion(regionDefinition, metadata, [&, completion](std::exception_ptr exception, mbgl::optional<mbgl::OfflineRegion> region) {
        NSError *error;
        if (exception) {
            NSString *errorDescription = @(mbgl::util::toString(exception).c_str());
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:errorDescription ? @{
                NSLocalizedDescriptionKey: errorDescription,
            } : nil];
        }
        if (completion) {
            MGLDownloadable *downloadable = [[MGLDownloadable alloc] initWithMBGLRegion:new mbgl::OfflineRegion(std::move(*region))];
            dispatch_async(dispatch_get_main_queue(), [&, completion, error, downloadable](void) {
                completion(downloadable, error);
            });
        }
    });
}

- (void)removeDownloadable:(MGLDownloadable *)downloadable withCompletionHandler:(MGLDownloadableRemovalCompletionHandler)completion {
    self.mbglFileSource->deleteOfflineRegion(std::move(*downloadable.mbglOfflineRegion), [&, completion](std::exception_ptr exception) {
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

- (void)requestDownloadablesWithCompletionHandler:(MGLDownloadablesRequestCompletionHandler)completion {
    self.mbglFileSource->listOfflineRegions([&, completion](std::exception_ptr exception, mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) {
        NSError *error;
        if (exception) {
            error = [NSError errorWithDomain:MGLErrorDomain code:-1 userInfo:@{
                NSLocalizedDescriptionKey: @(mbgl::util::toString(exception).c_str()),
            }];
        }
        NSMutableArray *downloadables;
        if (regions) {
            downloadables = [NSMutableArray arrayWithCapacity:regions->size()];
            for (mbgl::OfflineRegion &region : *regions) {
                MGLDownloadable *downloadable = [[MGLDownloadable alloc] initWithMBGLRegion:new mbgl::OfflineRegion(std::move(region))];
                [downloadables addObject:downloadable];
            }
        }
        if (completion) {
            dispatch_async(dispatch_get_main_queue(), [&, completion, error, downloadables](void) {
                completion(downloadables, error);
            });
        }
    });
}

- (void)setMaximumAllowedMapboxTiles:(uint64_t)maximumCount {
    _mbglFileSource->setOfflineMapboxTileCountLimit(maximumCount);
}

@end
