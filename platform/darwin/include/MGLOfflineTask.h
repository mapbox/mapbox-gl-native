#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflineTaskDelegate;

/**
 The state an offline task is currently in.
 */
typedef NS_ENUM (NSInteger, MGLOfflineTaskState) {
    /**
     It is unknown whether the task is inactive, active, or complete.
     
     This is the initial state of a task that is obtained using the
     `-[MGLOfflineStorage getTasksWithCompletionHandler:]` method. The state
     becomes known by the time the task’s delegate receives its first progress
     update. For inactive tasks, you must explicitly request a progress update
     using the `-[MGLOfflineTask requestProgress]` method.
     
     An invalid task always has a state of `MGLOfflineTaskStateInvalid`, never
     `MGLOfflineTaskStateUnknown`.
     */
    MGLOfflineTaskStateUnknown = 0,
    /**
     The task is incomplete and is not currently downloading.
     
     This is the initial state of a task that is created using the
     `-[MGLOfflineTask addTaskForRegion:withContext:completionHandler:]` method,
     as well as after the `-[MGLOfflineTask suspend]` method is
     called.
     */
    MGLOfflineTaskStateInactive = 1,
    /**
     The task is incomplete and is currently downloading.
     
     This is the state of a task after the `-[MGLOfflineTask resume]` method is
     called.
     */
    MGLOfflineTaskStateActive = 2,
    /**
     The task has downloaded to completion.
     */
    MGLOfflineTaskStateComplete = 3,
    /**
     The task has been removed using the
     `-[MGLOfflineStorage removeTask:withCompletionHandler:]` method. Sending
     any message to the task will raise an exception.
     */
    MGLOfflineTaskStateInvalid = 4,
};

/**
 A structure containing information about an offline task’s current download
 progress.
 */
typedef struct MGLOfflineTaskProgress {
    /**
     The number of resources that have been completely downloaded and are ready
     to use offline.
     */
    uint64_t countOfResourcesCompleted;
    /**
     The cumulative size of the downloaded resources on disk, measured in bytes.
     */
    uint64_t countOfBytesCompleted;
    /**
     The minimum number of resources that must be downloaded in order to view
     the task’s full region without any omissions.
     
     At the beginning of a download, this count is a lower bound; the number of
     expected resources may increase as the download progresses.
     */
    uint64_t countOfResourcesExpected;
    /**
     The maximum number of resources that must be downloaded in order to view
     the task’s full region without any omissions.
     
     At the beginning of a download, when the exact number of required resources
     is unknown, this field is set to `UINT64_MAX`. Thus this count is always an
     upper bound.
     */
    uint64_t maximumResourcesExpected;
} MGLOfflineTaskProgress;

/**
 An `MGLOfflineTask` writes the resources necessary for viewing a region offline
 to a local database, providing an optional `MGLOfflineTaskDelegate` object with
 progress updates as data or errors arrive from the server.
 */
@interface MGLOfflineTask : NSObject

/**
 The region for which the task manages resources.
 */
@property (nonatomic, readonly) id <MGLOfflineRegion> region;

/**
 Arbitrary data stored alongside the downloaded resources.
 
 The context typically holds application-specific information for identifying
 the task, such as a user-selected name.
 */
@property (nonatomic, readonly) NSData *context;

/**
 The task’s current state.
 
 The state of an inactive or completed task is computed lazily and is set to
 `MGLOfflineTaskStateUnknown` by default. If you need the state of a task
 inside an `MGLOfflineTaskListingCompletionHandler`, set the `delegate` property
 then call the `-requestProgress` method.
 */
@property (nonatomic, readonly) MGLOfflineTaskState state;

/**
 The task’s current progress.
 
 The progress of an inactive or completed task is computed lazily, and all its
 fields are set to 0 by default. If you need the progress of a task inside an
 `MGLOfflineTaskListingCompletionHandler`, set the `delegate` property then call
 the `-requestProgress` method.
 */
@property (nonatomic, readonly) MGLOfflineTaskProgress progress;

/**
 The task’s delegate.
 
 You can use the offline task delegate to be notified of any changes in the
 task’s progress and of any errors while downloading. For more information, see
 the `MGLOfflineTaskDelegate` documentation.
 */
@property (nonatomic, weak, nullable) id <MGLOfflineTaskDelegate> delegate;

- (instancetype)init NS_UNAVAILABLE;

/**
 Resumes downloading if the task is inactive.
 */
- (void)resume;

/**
 Temporarily stops downloading if the task is active.
 
 To resume downloading, call the `-resume` method.
 */
- (void)suspend;

/**
 Request an asynchronous update to the task’s `state` and `progress` properties.
 
 The state and progress of an inactive or completed task are computed lazily. If
 you need the state or progress of a task inside an
 `MGLOfflineTaskListingCompletionHandler`, set the `delegate` property then call
 this method.
 */
- (void)requestProgress;

@end

/**
 The `MGLOfflineTaskDelegate` protocol defines methods that a delegate of an
 `MGLOfflineTask` object can optionally implement to be notified of any changes
 in the task’s download progress and of any errors while downloading.
 */
@protocol MGLOfflineTaskDelegate <NSObject>

@optional

/**
 Sent whenever the task’s state or download progress changes. Every change to a
 field in the `progress` property corresponds to an invocation of this method.
 
 @param task The task whose state of progress changed.
 @param progress The updated progress. To get the updated state, refer to the
    `state` property.
 */
- (void)offlineTask:(MGLOfflineTask *)task progressDidChange:(MGLOfflineTaskProgress)progress;

/**
 Sent whenever the task encounters an error while downloading.
 
 Download errors may be recoverable. For example, this task’s implementation may
 attempt to re-request failed resources based on an exponential backoff
 strategy or upon the restoration of network access.
 
 @param task The task that encountered an error.
 @param error A download error. For a list of possible error codes, see
    `MGLErrorCode`.
 */
- (void)offlineTask:(MGLOfflineTask *)task didReceiveError:(NSError *)error;

/**
 Sent when the maximum number of Mapbox-hosted tiles has been downloaded and
 stored on the current device.
 
 Once this limit is reached, no instance of `MGLOfflineTask` can download
 additional tiles from Mapbox APIs until already downloaded tiles are removed by
 calling the `-[MGLOfflineStorage removeTask:withCompletionHandler:]` method.
 Contact your Mapbox sales representative to have the limit raised.
 */
- (void)offlineTask:(MGLOfflineTask *)task didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
