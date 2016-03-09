#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLOfflineTask;
@protocol MGLOfflineRegion;

/**
 A block to be called once an offline task has been completely created and
 added.
 
 @param task Contains a pointer to the newly added task, or `nil` if there was
    an error creating or adding the task.
 @param error Contains a pointer to an error object (if any) indicating why the
    task could not be created or added. For a list of possible error codes, see
    `MGLErrorCode`.
 */
typedef void (^MGLOfflineTaskAdditionCompletionHandler)(MGLOfflineTask * _Nullable task, NSError * _Nullable error);

/**
 A block to be called once an offline task has been completely invalidated and
 removed.
 
 @param error Contains a pointer to an error object (if any) indicating why the
    task could not be invalidated or removed.
 */
typedef void (^MGLOfflineTaskRemovalCompletionHandler)(NSError * _Nullable error);

/**
 A block to be called with a complete list of offline tasks.
 
 @param task Contains a pointer an array of tasks, or `nil` if there was an
    error obtaining the tasks.
 @param error Contains a pointer to an error object (if any) indicating why the
    list of tasks could not be obtained.
 */
typedef void (^MGLOfflineTaskListingCompletionHandler)(NS_ARRAY_OF(MGLOfflineTask *) *tasks, NSError * _Nullable error);

/**
 MGLOfflineStorage implements a singleton (shared object) that manages offline
 tasks. All of this class’s instance methods are asynchronous, reflecting the
 fact that offline resources are stored in a database.
 */
@interface MGLOfflineStorage : NSObject

/**
 Returns the shared offline storage object.
 */
+ (instancetype)sharedOfflineStorage;

- (instancetype)init NS_UNAVAILABLE;

/**
 Creates and registers an offline task that downloads the resources needed to
 use the given region offline.
 
 The resulting task starts out with a state of `MGLOfflineTaskStateInactive`. To
 begin downloading resources, call `-[MGLOfflineTask resume]`. To monitor
 download progress, set the task’s `delegate` property to an object that
 conforms to the `MGLOfflineTaskDelegate` protocol.
 
 @param region A region to download.
 @param context Arbitrary data to store alongside the downloaded resources.
 @param completion The completion handler to call once the task has been added.
    This handler is executed asynchronously on the main queue.
 */
- (void)addTaskForRegion:(id <MGLOfflineRegion>)region withContext:(NSData *)context completionHandler:(MGLOfflineTaskAdditionCompletionHandler)completion;

/**
 Unregisters the given offline task and frees any resources that are no longer
 required by any remaining tasks.
 
 As soon as this method is called on a task, the task becomes invalid; any
 attempt to send it a message will result in an exception being thrown. If an
 error occurs and the task cannot be removed, do not attempt to reuse the task
 object. Instead, use the `-getTasksWithCompletionHandler:` method to obtain a
 valid pointer to the task object.
 
 @param task The offline task to remove.
 @param completion The completion handler to call once the task has been
    removed. This handler is executed asynchronously on the main queue.
 */
- (void)removeTask:(MGLOfflineTask *)task withCompletionHandler:(MGLOfflineTaskRemovalCompletionHandler)completion;

/**
 Asynchronously calls a completion callback with all existing offline tasks.
 
 @param completion The completion handler to call with the list of tasks. This
     handler is executed asynchronously on the main queue.
 */
- (void)getTasksWithCompletionHandler:(MGLOfflineTaskListingCompletionHandler)completion;

/**
 Sets the maximum number of Mapbox-hosted tiles that may be downloaded and
 stored on the current device.
 
 Once this limit is reached,
 `-[MGLOfflineTaskDelegate offlineTask:didReceiveMaximumAllowedMapboxTiles:]` is
 called on every delegate of `MGLOfflineTask` until already downloaded tiles are
 removed by calling the `-removeTask:withCompletionHandler:` method.
 
 @note The [Mapbox Terms of Service](https://www.mapbox.com/tos/) prohibits
    changing or bypassing this limit without permission from Mapbox. Contact
    your Mapbox sales representative to have the limit raised.
 */
- (void)setMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
