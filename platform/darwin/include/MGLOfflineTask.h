#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflineTaskDelegate;

typedef NS_ENUM (NSInteger, MGLOfflineTaskState) {
    MGLOfflineTaskStateInactive = 0,
    MGLOfflineTaskStateActive = 1,
    MGLOfflineTaskStateComplete = 2,
};

typedef struct MGLOfflineTaskProgress {
    uint64_t countOfResourcesCompleted;
    uint64_t countOfBytesCompleted;
    uint64_t countOfResourcesExpected;
    uint64_t maximumResourcesExpected;
} MGLOfflineTaskProgress;

@interface MGLOfflineTask : NSObject

@property (nonatomic, readonly) id <MGLOfflineRegion> region;
@property (nonatomic, readonly) NSData *context;
@property (nonatomic, readonly) MGLOfflineTaskState state;
@property (nonatomic, readonly) MGLOfflineTaskProgress progress;
@property (nonatomic, weak, nullable) id <MGLOfflineTaskDelegate> delegate;

- (instancetype)init NS_UNAVAILABLE;

- (void)resume;
- (void)suspend;

- (void)requestProgress;

@end

@protocol MGLOfflineTaskDelegate <NSObject>

@optional

- (void)offlineTask:(MGLOfflineTask *)task progressDidChange:(MGLOfflineTaskProgress)progress;
- (void)offlineTask:(MGLOfflineTask *)task didReceiveError:(NSError *)error;
- (void)offlineTask:(MGLOfflineTask *)task didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
