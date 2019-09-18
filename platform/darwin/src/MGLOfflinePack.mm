#import "MGLOfflinePack_Private.h"

#import "MGLOfflineStorage_Private.h"
#import "MGLOfflineRegion_Private.h"
#import "MGLTilePyramidOfflineRegion.h"
#import "MGLTilePyramidOfflineRegion_Private.h"
#import "MGLShapeOfflineRegion.h"
#import "MGLShapeOfflineRegion_Private.h"
#import "MGLLoggingConfiguration_Private.h"

#import "NSValue+MGLAdditions.h"

#include <mbgl/map/map_options.hpp>
#include <mbgl/storage/default_file_source.hpp>

const MGLExceptionName MGLInvalidOfflinePackException = @"MGLInvalidOfflinePackException";

/**
 Assert that the current offline pack is valid.

 This macro should be used at the beginning of any public-facing instance method
 of `MGLOfflinePack`. For private methods, an assertion is more appropriate.
 */
#define MGLAssertOfflinePackIsValid() \
    do { \
        if (_state == MGLOfflinePackStateInvalid) { \
            [NSException raise:MGLInvalidOfflinePackException \
                        format: \
             @"-[MGLOfflineStorage removePack:withCompletionHandler:] has been called " \
             @"on this instance of MGLOfflinePack, rendering it invalid. It is an " \
             @"error to send any message to this pack."]; \
        } \
    } while (NO);

@interface MGLTilePyramidOfflineRegion () <MGLOfflineRegion_Private, MGLTilePyramidOfflineRegion_Private>
@end

@interface MGLShapeOfflineRegion () <MGLOfflineRegion_Private, MGLShapeOfflineRegion_Private>
@end

class MBGLOfflineRegionObserver : public mbgl::OfflineRegionObserver {
public:
    MBGLOfflineRegionObserver(MGLOfflinePack *pack_) : pack(pack_) {}

    void statusChanged(mbgl::OfflineRegionStatus status) override;
    void responseError(mbgl::Response::Error error) override;
    void mapboxTileCountLimitExceeded(uint64_t limit) override;

private:
    __weak MGLOfflinePack *pack = nullptr;
};

@interface MGLOfflinePack ()

@property (nonatomic, nullable, readwrite) mbgl::OfflineRegion *mbglOfflineRegion;
@property (nonatomic, readwrite) MGLOfflinePackProgress progress;

@end

@implementation MGLOfflinePack {
    BOOL _isSuspending;
    std::shared_ptr<mbgl::DefaultFileSource> _mbglFileSource;
}

- (instancetype)init {
    MGLLogInfo(@"Calling this initializer is not allowed.");
    if (self = [super init]) {
        _state = MGLOfflinePackStateInvalid;
        NSLog(@"%s called; did you mean to call +[MGLOfflineStorage addPackForRegion:withContext:completionHandler:] instead?", __PRETTY_FUNCTION__);
    }
    return self;
}

- (instancetype)initWithMBGLRegion:(mbgl::OfflineRegion *)region {
    if (self = [super init]) {
        _mbglOfflineRegion = region;
        _state = MGLOfflinePackStateUnknown;

        _mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];
        _mbglFileSource->setOfflineRegionObserver(*_mbglOfflineRegion, std::make_unique<MBGLOfflineRegionObserver>(self));
    }
    return self;
}

- (void)dealloc {
    MGLAssert(_state == MGLOfflinePackStateInvalid, @"MGLOfflinePack was not invalided prior to deallocation.");
}

- (id <MGLOfflineRegion>)region {
    MGLAssertOfflinePackIsValid();

    const mbgl::OfflineRegionDefinition &regionDefinition = _mbglOfflineRegion->getDefinition();
    MGLAssert([MGLTilePyramidOfflineRegion conformsToProtocol:@protocol(MGLOfflineRegion_Private)], @"MGLTilePyramidOfflineRegion should conform to MGLOfflineRegion_Private.");
    MGLAssert([MGLShapeOfflineRegion conformsToProtocol:@protocol(MGLOfflineRegion_Private)], @"MGLShapeOfflineRegion should conform to MGLOfflineRegion_Private.");
    
    
    
    return regionDefinition.match(
                           [&] (const mbgl::OfflineTilePyramidRegionDefinition def){
                               return (id <MGLOfflineRegion>)[[MGLTilePyramidOfflineRegion alloc] initWithOfflineRegionDefinition:def];
                           },
                           [&] (const mbgl::OfflineGeometryRegionDefinition& def){
                               return (id <MGLOfflineRegion>)[[MGLShapeOfflineRegion alloc] initWithOfflineRegionDefinition:def];
                           });
}

- (NSData *)context {
    MGLAssertOfflinePackIsValid();

    const mbgl::OfflineRegionMetadata &metadata = _mbglOfflineRegion->getMetadata();
    return [NSData dataWithBytes:&metadata[0] length:metadata.size()];
}

- (void)resume {
    MGLLogInfo(@"Resuming pack download.");
    MGLAssertOfflinePackIsValid();

    self.state = MGLOfflinePackStateActive;

    _mbglFileSource->setOfflineRegionDownloadState(*_mbglOfflineRegion, mbgl::OfflineRegionDownloadState::Active);
}

- (void)suspend {
    MGLLogInfo(@"Suspending pack download.");
    MGLAssertOfflinePackIsValid();

    if (self.state == MGLOfflinePackStateActive) {
        self.state = MGLOfflinePackStateInactive;
        _isSuspending = YES;
    }

    _mbglFileSource->setOfflineRegionDownloadState(*_mbglOfflineRegion, mbgl::OfflineRegionDownloadState::Inactive);
}

- (void)invalidate {
    MGLLogInfo(@"Invalidating pack.");
    MGLAssert(_state != MGLOfflinePackStateInvalid, @"Cannot invalidate an already invalid offline pack.");
    MGLAssert(self.mbglOfflineRegion, @"Should have a valid region");

    @synchronized (self) {
        self.state = MGLOfflinePackStateInvalid;
        if (self.mbglOfflineRegion) {
            _mbglFileSource->setOfflineRegionObserver(*self.mbglOfflineRegion, nullptr);
        }
        self.mbglOfflineRegion = nil;
    }
}

- (void)setState:(MGLOfflinePackState)state {
    MGLLogDebug(@"Setting state: %ld", (long)state);
    if (!self.mbglOfflineRegion) {
        // A progress update has arrived after the call to
        // -[MGLOfflineStorage removePack:withCompletionHandler:] but before the
        // removal is complete and the completion handler is called.
        MGLAssert(_state == MGLOfflinePackStateInvalid, @"A valid MGLOfflinePack has no mbgl::OfflineRegion.");
        return;
    }

    MGLAssert(_state != MGLOfflinePackStateInvalid, @"Cannot change the state of an invalid offline pack.");

    if (!_isSuspending || state != MGLOfflinePackStateActive) {
        _isSuspending = NO;
        _state = state;
    }
}

- (void)requestProgress {
    MGLLogInfo(@"Requesting pack progress.");
    MGLAssertOfflinePackIsValid();

    __weak MGLOfflinePack *weakSelf = self;
    _mbglFileSource->getOfflineRegionStatus(*_mbglOfflineRegion, [&, weakSelf](mbgl::expected<mbgl::OfflineRegionStatus, std::exception_ptr> status) {
        if (status) {
            mbgl::OfflineRegionStatus checkedStatus = *status;
            dispatch_async(dispatch_get_main_queue(), ^{
                MGLOfflinePack *strongSelf = weakSelf;
                [strongSelf offlineRegionStatusDidChange:checkedStatus];
            });
        }
    });
}

- (void)offlineRegionStatusDidChange:(mbgl::OfflineRegionStatus)status {
    MGLAssert(_state != MGLOfflinePackStateInvalid, @"Cannot change update progress of an invalid offline pack.");

    switch (status.downloadState) {
        case mbgl::OfflineRegionDownloadState::Inactive:
            self.state = status.complete() ? MGLOfflinePackStateComplete : MGLOfflinePackStateInactive;
            break;

        case mbgl::OfflineRegionDownloadState::Active:
            self.state = MGLOfflinePackStateActive;
            break;
    }

    if (_isSuspending) {
        return;
    }

    MGLOfflinePackProgress progress;
    progress.countOfResourcesCompleted = status.completedResourceCount;
    progress.countOfBytesCompleted = status.completedResourceSize;
    progress.countOfTilesCompleted = status.completedTileCount;
    progress.countOfTileBytesCompleted = status.completedTileSize;
    progress.countOfResourcesExpected = status.requiredResourceCount;
    progress.maximumResourcesExpected = status.requiredResourceCountIsPrecise ? status.requiredResourceCount : UINT64_MAX;
    self.progress = progress;

    NSDictionary *userInfo = @{MGLOfflinePackUserInfoKeyState: @(self.state),
                               MGLOfflinePackUserInfoKeyProgress: [NSValue valueWithMGLOfflinePackProgress:progress]};

    NSNotificationCenter *noteCenter = [NSNotificationCenter defaultCenter];
    [noteCenter postNotificationName:MGLOfflinePackProgressChangedNotification
                              object:self
                            userInfo:userInfo];
}

- (void)didReceiveError:(NSError *)error {
    MGLLogError(@"Error: %@", error.localizedDescription);
    MGLLogInfo(@"Notifying about pack error.");
    
    NSDictionary *userInfo = @{ MGLOfflinePackUserInfoKeyError: error };
    NSNotificationCenter *noteCenter = [NSNotificationCenter defaultCenter];
    [noteCenter postNotificationName:MGLOfflinePackErrorNotification
                              object:self
                            userInfo:userInfo];
}

- (void)didReceiveMaximumAllowedMapboxTiles:(uint64_t)limit {
    MGLLogInfo(@"Notifying reached maximum allowed Mapbox tiles: %lu", (unsigned long)limit);
    NSDictionary *userInfo = @{ MGLOfflinePackUserInfoKeyMaximumCount: @(limit) };
    NSNotificationCenter *noteCenter = [NSNotificationCenter defaultCenter];
    [noteCenter postNotificationName:MGLOfflinePackMaximumMapboxTilesReachedNotification
                              object:self
                            userInfo:userInfo];
}

NSError *MGLErrorFromResponseError(mbgl::Response::Error error) {
    NSInteger errorCode = MGLErrorCodeUnknown;
    switch (error.reason) {
        case mbgl::Response::Error::Reason::NotFound:
            errorCode = MGLErrorCodeNotFound;
            break;

        case mbgl::Response::Error::Reason::Server:
            errorCode = MGLErrorCodeBadServerResponse;
            break;

        case mbgl::Response::Error::Reason::Connection:
            errorCode = MGLErrorCodeConnectionFailed;
            break;

        default:
            break;
    }
    return [NSError errorWithDomain:MGLErrorDomain code:errorCode userInfo:@{
        NSLocalizedFailureReasonErrorKey: @(error.message.c_str())
    }];
}

@end

void MBGLOfflineRegionObserver::statusChanged(mbgl::OfflineRegionStatus status) {
    __weak MGLOfflinePack *weakPack = pack;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakPack offlineRegionStatusDidChange:status];
    });
}

void MBGLOfflineRegionObserver::responseError(mbgl::Response::Error error) {
    __weak MGLOfflinePack *weakPack = pack;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakPack didReceiveError:MGLErrorFromResponseError(error)];
    });
}

void MBGLOfflineRegionObserver::mapboxTileCountLimitExceeded(uint64_t limit) {
    __weak MGLOfflinePack *weakPack = pack;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakPack didReceiveMaximumAllowedMapboxTiles:limit];
    });
}
