#import "MGLOfflinePack.h"

#include <mbgl/storage/default_file_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLOfflinePackDelegate;

@interface MGLOfflinePack (Private)

/**
 The pack’s delegate.
 
 You can use the offline pack delegate to be notified of any changes in the
 pack’s progress and of any errors while downloading. For more information, see
 the `MGLOfflinePackDelegate` documentation.
 */
@property (nonatomic, weak, nullable) id <MGLOfflinePackDelegate> delegate;

@property (nonatomic, nullable) mbgl::OfflineRegion *mbglOfflineRegion;

@property (nonatomic, readwrite) MGLOfflinePackState state;

- (instancetype)initWithMBGLRegion:(mbgl::OfflineRegion *)region;

/**
 Invalidates the pack and ensures that no future progress update can ever
 revalidate it. This method must be called before the pack is deallocated.
 */
- (void)invalidate;

@end

/**
 The `MGLOfflinePackDelegate` protocol defines methods that a delegate of an
 `MGLOfflinePack` object can optionally implement to be notified of any changes
 in the pack’s download progress and of any errors while downloading.
 */
@protocol MGLOfflinePackDelegate <NSObject>

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
