#import <Foundation/Foundation.h>

#import "MGLDownloadRegion.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLDownloadableDelegate;

typedef NS_ENUM (NSInteger, MGLDownloadableState) {
    MGLDownloadableStateInactive = 0,
    MGLDownloadableStateActive = 1,
    MGLDownloadableStateComplete = 2,
};

typedef struct MGLDownloadableProgress {
    uint64_t countOfResourcesCompleted;
    uint64_t countOfBytesCompleted;
    uint64_t countOfResourcesExpected;
    uint64_t maximumResourcesExpected;
} MGLDownloadableProgress;

@interface MGLDownloadable : NSObject

@property (nonatomic, readonly) id <MGLDownloadRegion> region;
@property (nonatomic, readonly) NSData *context;
@property (nonatomic, readonly) MGLDownloadableState state;
@property (nonatomic, readonly) MGLDownloadableProgress progress;
@property (nonatomic, weak, nullable) id <MGLDownloadableDelegate> delegate;

- (instancetype)init NS_UNAVAILABLE;

- (void)resume;
- (void)suspend;

- (void)requestProgress;

@end

@protocol MGLDownloadableDelegate <NSObject>

@optional

- (void)downloadable:(MGLDownloadable *)downloadable progressDidChange:(MGLDownloadableProgress)progress;
- (void)downloadable:(MGLDownloadable *)downloadable didReceiveError:(NSError *)error;
- (void)downloadable:(MGLDownloadable *)downloadable didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
