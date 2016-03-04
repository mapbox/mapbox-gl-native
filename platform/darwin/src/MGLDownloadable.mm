#import "MGLDownloadable_Private.h"

#import "MGLDownloadController_Private.h"
#import "MGLDownloadRegion_Private.h"
#import "MGLTilePyramidDownloadRegion.h"

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/string.hpp>

class MBGLOfflineRegionObserver;

@interface MGLDownloadable ()

@property (nonatomic, readwrite) mbgl::OfflineRegion *mbglOfflineRegion;
@property (nonatomic, readwrite) MBGLOfflineRegionObserver *mbglOfflineRegionObserver;
@property (nonatomic, readwrite) MGLDownloadableState state;
@property (nonatomic, readwrite) MGLDownloadableProgress progress;

@end

@implementation MGLDownloadable

- (instancetype)init {
    [NSException raise:@"Method unavailable"
                format:
     @"-[MGLDownloadable init] is unavailable. "
     @"Use +[MGLDownloadController addDownloadRegion:context:completionHandler:] instead."];
    return nil;
}

- (instancetype)initWithMBGLRegion:(mbgl::OfflineRegion *)region {
    if (self = [super init]) {
        _mbglOfflineRegion = region;
        _state = MGLDownloadableStateInactive;
        _mbglOfflineRegionObserver = new MBGLOfflineRegionObserver(self);
        
        mbgl::DefaultFileSource *mbglFileSource = [[MGLDownloadController sharedController] mbglFileSource];
        mbglFileSource->setOfflineRegionObserver(*_mbglOfflineRegion, std::make_unique<MBGLOfflineRegionObserver>(*_mbglOfflineRegionObserver));
    }
    return self;
}

- (void)dealloc {
    delete _mbglOfflineRegionObserver;
    _mbglOfflineRegionObserver = nullptr;
}

- (id <MGLDownloadRegion>)region {
    const mbgl::OfflineRegionDefinition &regionDefinition = _mbglOfflineRegion->getDefinition();
    NSAssert([MGLTilePyramidDownloadRegion conformsToProtocol:@protocol(MGLDownloadRegion_Private)], @"MGLTilePyramidDownloadRegion should conform to MGLDownloadRegion_Private.");
    return [(id <MGLDownloadRegion_Private>)[MGLTilePyramidDownloadRegion alloc] initWithOfflineRegionDefinition:regionDefinition];
}

- (NSData *)context {
    const mbgl::OfflineRegionMetadata &metadata = _mbglOfflineRegion->getMetadata();
    return [NSData dataWithBytes:&metadata[0] length:metadata.size()];
}

- (void)resume {
    mbgl::DefaultFileSource *mbglFileSource = [[MGLDownloadController sharedController] mbglFileSource];
    mbglFileSource->setOfflineRegionDownloadState(*_mbglOfflineRegion, mbgl::OfflineRegionDownloadState::Active);
    self.state = MGLDownloadableStateActive;
}

- (void)suspend {
    mbgl::DefaultFileSource *mbglFileSource = [[MGLDownloadController sharedController] mbglFileSource];
    mbglFileSource->setOfflineRegionDownloadState(*_mbglOfflineRegion, mbgl::OfflineRegionDownloadState::Inactive);
    self.state = MGLDownloadableStateInactive;
}

- (void)requestProgress {
    mbgl::DefaultFileSource *mbglFileSource = [[MGLDownloadController sharedController] mbglFileSource];
    
    __weak MGLDownloadable *weakSelf = self;
    mbglFileSource->getOfflineRegionStatus(*_mbglOfflineRegion, [&, weakSelf](__unused std::exception_ptr exception, mbgl::optional<mbgl::OfflineRegionStatus> status) {
        if (status) {
            mbgl::OfflineRegionStatus checkedStatus = *status;
            dispatch_async(dispatch_get_main_queue(), ^{
                MGLDownloadable *strongSelf = weakSelf;
                [strongSelf offlineRegionStatusDidChange:checkedStatus];
            });
        }
    });
}

- (void)offlineRegionStatusDidChange:(mbgl::OfflineRegionStatus)status {
    switch (status.downloadState) {
        case mbgl::OfflineRegionDownloadState::Inactive:
            self.state = status.complete() ? MGLDownloadableStateComplete : MGLDownloadableStateInactive;
            break;
            
        case mbgl::OfflineRegionDownloadState::Active:
            self.state = MGLDownloadableStateActive;
            break;
    }
    
    MGLDownloadableProgress progress;
    progress.countOfResourcesCompleted = status.completedResourceCount;
    progress.countOfBytesCompleted = status.completedResourceSize;
    progress.countOfResourcesExpected = status.requiredResourceCount;
    progress.maximumResourcesExpected = status.requiredResourceCountIsPrecise ? status.requiredResourceCount : UINT64_MAX;
    self.progress = progress;
    
    if ([self.delegate respondsToSelector:@selector(downloadable:progressDidChange:)]) {
        [self.delegate downloadable:self progressDidChange:progress];
    }
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

void MBGLOfflineRegionObserver::statusChanged(mbgl::OfflineRegionStatus status) {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSCAssert(downloadable, @"MBGLOfflineRegionObserver is dangling without an associated MGLDownloadable.");
        
        [downloadable offlineRegionStatusDidChange:status];
    });
}

void MBGLOfflineRegionObserver::responseError(mbgl::Response::Error error) {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSCAssert(downloadable, @"MBGLOfflineRegionObserver is dangling without an associated MGLDownloadable.");
        
        if ([downloadable.delegate respondsToSelector:@selector(downloadable:didReceiveError:)]) {
            [downloadable.delegate downloadable:downloadable didReceiveError:MGLErrorFromResponseError(error)];
        }
    });
}

void MBGLOfflineRegionObserver::mapboxTileCountLimitExceeded(uint64_t limit) {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSCAssert(downloadable, @"MBGLOfflineRegionObserver is dangling without an associated MGLDownloadable.");
        
        if ([downloadable.delegate respondsToSelector:@selector(downloadable:didReceiveMaximumAllowedMapboxTiles:)]) {
            [downloadable.delegate downloadable:downloadable didReceiveMaximumAllowedMapboxTiles:limit];
        }
    });
}

@end
