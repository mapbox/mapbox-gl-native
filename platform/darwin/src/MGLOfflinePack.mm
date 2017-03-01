#import "MGLOfflinePack_Private.h"

#import "MGLOfflineStorage_Private.h"
#import "MGLOfflineRegion_Private.h"
#import "MGLTilePyramidOfflineRegion.h"

#import "NSValue+MGLAdditions.h"

#include <mbgl/storage/default_file_source.hpp>

/**
 Assert that the current offline pack is valid.

 This macro should be used at the beginning of any public-facing instance method
 of `MGLOfflinePack`. For private methods, an assertion is more appropriate.
 */
#define MGLAssertOfflinePackIsValid() \
    do { \
        if (_state == MGLOfflinePackStateInvalid) { \
            [NSException raise:@"Invalid offline pack" \
                        format: \
             @"-[MGLOfflineStorage removePack:withCompletionHandler:] has been called " \
             @"on this instance of MGLOfflinePack, rendering it invalid. It is an " \
             @"error to send any message to this pack."]; \
        } \
    } while (NO);

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
}

- (instancetype)init {
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

        mbgl::DefaultFileSource *mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];
        mbglFileSource->setOfflineRegionObserver(*_mbglOfflineRegion, std::make_unique<MBGLOfflineRegionObserver>(self));
    }
    return self;
}

- (void)dealloc {
    NSAssert(_state == MGLOfflinePackStateInvalid, @"MGLOfflinePack was not invalided prior to deallocation.");
}

- (id <MGLOfflineRegion>)region {
    MGLAssertOfflinePackIsValid();

    const mbgl::OfflineRegionDefinition &regionDefinition = _mbglOfflineRegion->getDefinition();
    NSAssert([MGLTilePyramidOfflineRegion conformsToProtocol:@protocol(MGLOfflineRegion_Private)], @"MGLTilePyramidOfflineRegion should conform to MGLOfflineRegion_Private.");
    return [(id <MGLOfflineRegion_Private>)[MGLTilePyramidOfflineRegion alloc] initWithOfflineRegionDefinition:regionDefinition];
}

- (NSData *)context {
    MGLAssertOfflinePackIsValid();

    const mbgl::OfflineRegionMetadata &metadata = _mbglOfflineRegion->getMetadata();
    return [NSData dataWithBytes:&metadata[0] length:metadata.size()];
}

- (void)resume {
    MGLAssertOfflinePackIsValid();

    self.state = MGLOfflinePackStateActive;

    mbgl::DefaultFileSource *mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];
    mbglFileSource->setOfflineRegionDownloadState(*_mbglOfflineRegion, mbgl::OfflineRegionDownloadState::Active);
}

- (void)suspend {
    MGLAssertOfflinePackIsValid();

    if (self.state == MGLOfflinePackStateActive) {
        self.state = MGLOfflinePackStateInactive;
        _isSuspending = YES;
    }

    mbgl::DefaultFileSource *mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];
    mbglFileSource->setOfflineRegionDownloadState(*_mbglOfflineRegion, mbgl::OfflineRegionDownloadState::Inactive);
}

- (void)invalidate {
    NSAssert(_state != MGLOfflinePackStateInvalid, @"Cannot invalidate an already invalid offline pack.");

    self.state = MGLOfflinePackStateInvalid;
    mbgl::DefaultFileSource *mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];
    mbglFileSource->setOfflineRegionObserver(*self.mbglOfflineRegion, nullptr);
    self.mbglOfflineRegion = nil;
}

- (void)setState:(MGLOfflinePackState)state {
    if (!self.mbglOfflineRegion) {
        // A progress update has arrived after the call to
        // -[MGLOfflineStorage removePack:withCompletionHandler:] but before the
        // removal is complete and the completion handler is called.
        NSAssert(_state == MGLOfflinePackStateInvalid, @"A valid MGLOfflinePack has no mbgl::OfflineRegion.");
        return;
    }

    NSAssert(_state != MGLOfflinePackStateInvalid, @"Cannot change the state of an invalid offline pack.");

    if (!_isSuspending || state != MGLOfflinePackStateActive) {
        _isSuspending = NO;
        _state = state;
    }
}

- (void)requestProgress {
    MGLAssertOfflinePackIsValid();

    mbgl::DefaultFileSource *mbglFileSource = [[MGLOfflineStorage sharedOfflineStorage] mbglFileSource];

    __weak MGLOfflinePack *weakSelf = self;
    mbglFileSource->getOfflineRegionStatus(*_mbglOfflineRegion, [&, weakSelf](__unused std::exception_ptr exception, mbgl::optional<mbgl::OfflineRegionStatus> status) {
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
    NSAssert(_state != MGLOfflinePackStateInvalid, @"Cannot change update progress of an invalid offline pack.");

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
    NSDictionary *userInfo = @{ MGLOfflinePackUserInfoKeyError: error };
    NSNotificationCenter *noteCenter = [NSNotificationCenter defaultCenter];
    [noteCenter postNotificationName:MGLOfflinePackErrorNotification
                              object:self
                            userInfo:userInfo];
}

- (void)didReceiveMaximumAllowedMapboxTiles:(uint64_t)limit {
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
    dispatch_async(dispatch_get_main_queue(), ^{
        [pack offlineRegionStatusDidChange:status];
    });
}

void MBGLOfflineRegionObserver::responseError(mbgl::Response::Error error) {
    dispatch_async(dispatch_get_main_queue(), ^{
        [pack didReceiveError:MGLErrorFromResponseError(error)];
    });
}

void MBGLOfflineRegionObserver::mapboxTileCountLimitExceeded(uint64_t limit) {
    dispatch_async(dispatch_get_main_queue(), ^{
        [pack didReceiveMaximumAllowedMapboxTiles:limit];
    });
}
