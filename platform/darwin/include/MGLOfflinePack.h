#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflinePackDelegate;

/**
 The state an offline pack is currently in.
 */
typedef NS_ENUM (NSInteger, MGLOfflinePackState) {
    /**
     It is unknown whether the pack is inactive, active, or complete.
     
     This is the initial state of a pack that is obtained using the
     `-[MGLOfflineStorage getPacksWithCompletionHandler:]` method. The state
     becomes known by the time the pack’s delegate receives its first progress
     update. For inactive packs, you must explicitly request a progress update
     using the `-[MGLOfflinePack requestProgress]` method.
     
     An invalid pack always has a state of `MGLOfflinePackStateInvalid`, never
     `MGLOfflinePackStateUnknown`.
     */
    MGLOfflinePackStateUnknown = 0,
    /**
     The pack is incomplete and is not currently downloading.
     
     This is the initial state of a pack that is created using the
     `-[MGLOfflinePack addPackForRegion:withContext:completionHandler:]` method,
     as well as after the `-[MGLOfflinePack suspend]` method is
     called.
     */
    MGLOfflinePackStateInactive = 1,
    /**
     The pack is incomplete and is currently downloading.
     
     This is the state of a pack after the `-[MGLOfflinePack resume]` method is
     called.
     */
    MGLOfflinePackStateActive = 2,
    /**
     The pack has downloaded to completion.
     */
    MGLOfflinePackStateComplete = 3,
    /**
     The pack has been removed using the
     `-[MGLOfflineStorage removePack:withCompletionHandler:]` method. Sending
     any message to the pack will raise an exception.
     */
    MGLOfflinePackStateInvalid = 4,
};

/**
 A structure containing information about an offline pack’s current download
 progress.
 */
typedef struct MGLOfflinePackProgress {
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
     the pack’s full region without any omissions.
     
     At the beginning of a download, this count is a lower bound; the number of
     expected resources may increase as the download progresses.
     */
    uint64_t countOfResourcesExpected;
    /**
     The maximum number of resources that must be downloaded in order to view
     the pack’s full region without any omissions.
     
     At the beginning of a download, when the exact number of required resources
     is unknown, this field is set to `UINT64_MAX`. Thus this count is always an
     upper bound.
     */
    uint64_t maximumResourcesExpected;
} MGLOfflinePackProgress;

/**
 An `MGLOfflinePack` represents a collection of resources necessary for viewing
 a region offline to a local database. It provides an optional
 `MGLOfflinePackDelegate` object with progress updates as data or errors arrive
 from the server.
 */
@interface MGLOfflinePack : NSObject

/**
 The region for which the pack manages resources.
 */
@property (nonatomic, readonly) id <MGLOfflineRegion> region;

/**
 Arbitrary data stored alongside the downloaded resources.
 
 The context typically holds application-specific information for identifying
 the pack, such as a user-selected name.
 */
@property (nonatomic, readonly) NSData *context;

/**
 The pack’s current state.
 
 The state of an inactive or completed pack is computed lazily and is set to
 `MGLOfflinePackStateUnknown` by default. If you need the state of a pack
 inside an `MGLOfflinePackListingCompletionHandler`, set the `delegate` property
 then call the `-requestProgress` method.
 */
@property (nonatomic, readonly) MGLOfflinePackState state;

/**
 The pack’s current progress.
 
 The progress of an inactive or completed pack is computed lazily, and all its
 fields are set to 0 by default. If you need the progress of a pack inside an
 `MGLOfflinePackListingCompletionHandler`, set the `delegate` property then call
 the `-requestProgress` method.
 */
@property (nonatomic, readonly) MGLOfflinePackProgress progress;

/**
 The pack’s delegate.
 
 You can use the offline pack delegate to be notified of any changes in the
 pack’s progress and of any errors while downloading. For more information, see
 the `MGLOfflinePackDelegate` documentation.
 */
@property (nonatomic, weak, nullable) id <MGLOfflinePackDelegate> delegate;

- (instancetype)init NS_UNAVAILABLE;

/**
 Resumes downloading if the pack is inactive.
 */
- (void)resume;

/**
 Temporarily stops downloading if the pack is active.
 
 To resume downloading, call the `-resume` method.
 */
- (void)suspend;

/**
 Request an asynchronous update to the pack’s `state` and `progress` properties.
 
 The state and progress of an inactive or completed pack are computed lazily. If
 you need the state or progress of a pack inside an
 `MGLOfflinePackListingCompletionHandler`, set the `delegate` property then call
 this method.
 */
- (void)requestProgress;

@end

/**
 The `MGLOfflinePackDelegate` protocol defines methods that a delegate of an
 `MGLOfflinePack` object can optionally implement to be notified of any changes
 in the pack’s download progress and of any errors while downloading.
 */
@protocol MGLOfflinePackDelegate <NSObject>

@optional

/**
 Sent whenever the pack’s state or download progress changes. Every change to a
 field in the `progress` property corresponds to an invocation of this method.
 
 @param pack The pack whose state of progress changed.
 @param progress The updated progress. To get the updated state, refer to the
    `state` property.
 */
- (void)offlinePack:(MGLOfflinePack *)pack progressDidChange:(MGLOfflinePackProgress)progress;

/**
 Sent whenever the pack encounters an error while downloading.
 
 Download errors may be recoverable. For example, this pack’s implementation may
 attempt to re-request failed resources based on an exponential backoff
 strategy or upon the restoration of network access.
 
 @param pack The pack that encountered an error.
 @param error A download error. For a list of possible error codes, see
    `MGLErrorCode`.
 */
- (void)offlinePack:(MGLOfflinePack *)pack didReceiveError:(NSError *)error;

/**
 Sent when the maximum number of Mapbox-hosted tiles has been downloaded and
 stored on the current device.
 
 Once this limit is reached, no instance of `MGLOfflinePack` can download
 additional tiles from Mapbox APIs until already downloaded tiles are removed by
 calling the `-[MGLOfflineStorage removePack:withCompletionHandler:]` method.
 Contact your Mapbox sales representative to have the limit raised.
 */
- (void)offlinePack:(MGLOfflinePack *)pack didReceiveMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
