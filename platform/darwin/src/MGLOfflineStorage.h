#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLOfflinePack;
@protocol MGLOfflineRegion;

/**
 Posted by the shared `MGLOfflineStorage` object when an `MGLOfflinePack`
 object’s progress changes. The progress may change due to a resource being
 downloaded or because the pack discovers during the download that more
 resources are required for offline viewing. This notification is posted
 whenever any field in the `progress` property changes.
 
 The `object` is the `MGLOfflinePack` object whose progress changed. The
 `userInfo` dictionary contains the pack’s current state in the
 `MGLOfflinePackStateUserInfoKey` key and details about the pack’s current
 progress in the `MGLOfflinePackProgressUserInfoKey` key. You may also consult
 the pack’s `state` and `progress` properties, which provide the same values.
 
 If you only need to observe changes in a particular pack’s progress, you can
 alternatively observe KVO change notifications to the pack’s `progress` key
 path.
 */
extern NSString * const MGLOfflinePackProgressChangedNotification;

/**
 Posted by the shared `MGLOfflineStorage` object whenever an `MGLOfflinePack`
 object encounters an error while downloading. The error may be recoverable and
 may not warrant the user’s attention. For example, the pack’s implementation
 may attempt to re-request failed resources based on an exponential backoff
 strategy or upon the restoration of network access.
 
 The `object` is the `MGLOfflinePack` object that encountered the error. The
 `userInfo` dictionary contains the error object in the
 `MGLOfflinePackErrorUserInfoKey` key.
 */
extern NSString * const MGLOfflinePackErrorNotification;

/**
 Posted by the shared `MGLOfflineStorage` object when the maximum number of
 Mapbox-hosted tiles has been downloaded and stored on the current device.
 
 The `object` is the `MGLOfflinePack` object that reached the tile limit in the
 course of downloading. The `userInfo` dictionary contains the tile limit in the
 `MGLOfflinePackMaximumCountUserInfoKey` key.
 
 Once this limit is reached, no instance of `MGLOfflinePack` can download
 additional tiles from Mapbox APIs until already downloaded tiles are removed by
 calling the `-[MGLOfflineStorage removePack:withCompletionHandler:]` method.
 Contact your Mapbox sales representative to have the limit raised.
 */
extern NSString * const MGLOfflinePackMaximumMapboxTilesReachedNotification;

/**
 The key for an `NSNumber` object that indicates an offline pack’s current
 state. This key is used in the `userInfo` dictionary of an
 `MGLOfflinePackProgressChangedNotification` notification. Call `-integerValue`
 on the object to receive the `MGLOfflinePackState`-typed state.
 */
extern NSString * const MGLOfflinePackStateUserInfoKey;

/**
 The key for an `NSValue` object that indicates an offline pack’s current
 progress. This key is used in the `userInfo` dictionary of an
 `MGLOfflinePackProgressChangedNotification` notification. Call
 `-MGLOfflinePackProgressValue` on the object to receive the
 `MGLOfflinePackProgress`-typed progress.
 */
extern NSString * const MGLOfflinePackProgressUserInfoKey;

/**
 The key for an `NSError` object that is encountered in the course of
 downloading an offline pack. This key is used in the `userInfo` dictionary of
 an `MGLOfflinePackErrorNotification` notification. The error’s domain is
 `MGLErrorDomain`. See `MGLErrorCode` for possible error codes.
 */
extern NSString * const MGLOfflinePackErrorUserInfoKey;

/**
 The key for an `NSNumber` object that indicates the maximum number of
 Mapbox-hosted tiles that may be downloaded and stored on the current device.
 This key is used in the `userInfo` dictionary of an
 `MGLOfflinePackMaximumMapboxTilesReachedNotification` notification. Call
 `-unsignedLongLongValue` on the object to receive the `uint64_t`-typed tile
 limit.
 */
extern NSString * const MGLOfflinePackMaximumCountUserInfoKey;

/**
 A block to be called once an offline pack has been completely created and
 added.
 
 An application typically calls the `-resume` method on the pack inside this
 completion handler to begin the download.
 
 @param pack Contains a pointer to the newly added pack, or `nil` if there was
    an error creating or adding the pack.
 @param error Contains a pointer to an error object (if any) indicating why the
    pack could not be created or added.
 */
typedef void (^MGLOfflinePackAdditionCompletionHandler)(MGLOfflinePack * _Nullable pack, NSError * _Nullable error);

/**
 A block to be called once an offline pack has been completely invalidated and
 removed.
 
 Avoid any references to the pack inside this completion handler: by the time
 this completion handler is executed, the pack has become invalid, and any
 messages passed to it will raise an exception.
 
 @param error Contains a pointer to an error object (if any) indicating why the
    pack could not be invalidated or removed.
 */
typedef void (^MGLOfflinePackRemovalCompletionHandler)(NSError * _Nullable error);

/**
 MGLOfflineStorage implements a singleton (shared object) that manages offline
 packs. All of this class’s instance methods are asynchronous, reflecting the
 fact that offline resources are stored in a database. The shared object
 maintains a canonical collection of offline packs in its `packs` property.
 */
@interface MGLOfflineStorage : NSObject

/**
 Returns the shared offline storage object.
 */
+ (instancetype)sharedOfflineStorage;

/**
 An array of all known offline packs, in the order in which they were created.
 
 This property is set to `nil`, indicating that the receiver does not yet know
 the existing packs, for an undefined amount of time starting from the moment
 the shared offline storage object is initialized until the packs are fetched
 from the database. After that point, this property is always non-nil, but it
 may be empty to indicate that no packs are present.
 
 To detect when the shared offline storage object has finished loading its
 `packs` property, observe KVO change notifications on the `packs` key path.
 The initial load results in an `NSKeyValueChangeSetting` change.
 */
@property (nonatomic, strong, readonly, nullable) NS_ARRAY_OF(MGLOfflinePack *) *packs;

/**
 Creates and registers an offline pack that downloads the resources needed to
 use the given region offline.
 
 The resulting pack is added to the shared offline storage object’s `packs`
 property, then the `completion` block is executed with that pack passed in.
 
 The pack has an initial state of `MGLOfflinePackStateInactive`. To begin
 downloading resources, call `-[MGLOfflinePack resume]` on the pack from within
 the completion handler. To monitor download progress, add an observer for
 `MGLOfflinePackProgressChangedNotification`s about that pack.
 
 To detect when any call to this method results in a new pack, observe KVO
 change notifications on the shared offline storage object’s `packs` key path.
 Additions to that array result in an `NSKeyValueChangeInsertion` change.
 
 @param region A region to download.
 @param context Arbitrary data to store alongside the downloaded resources.
 @param completion The completion handler to call once the pack has been added.
    This handler is executed asynchronously on the main queue.
 */
- (void)addPackForRegion:(id <MGLOfflineRegion>)region withContext:(NSData *)context completionHandler:(nullable MGLOfflinePackAdditionCompletionHandler)completion;

/**
 Unregisters the given offline pack and frees any resources that are no longer
 required by any remaining packs.
 
 As soon as this method is called on a pack, the pack becomes invalid; any
 attempt to send it a message will result in an exception being thrown. If an
 error occurs and the pack cannot be removed, do not attempt to reuse the pack
 object. Instead, if you need continued access to the pack, suspend all packs
 and use the `-reloadPacks` method to obtain valid pointers to all the packs.
 
 To detect when any call to this method results in a pack being removed, observe
 KVO change notifications on the shared offline storage object’s `packs` key
 path. Removals from that array result in an `NSKeyValueChangeRemoval` change.
 
 @param pack The offline pack to remove.
 @param completion The completion handler to call once the pack has been
    removed. This handler is executed asynchronously on the main queue.
 */
- (void)removePack:(MGLOfflinePack *)pack withCompletionHandler:(nullable MGLOfflinePackRemovalCompletionHandler)completion;

/**
 Forcibly, asynchronously reloads the `packs` property. At some point after this
 method is called, the pointer values of the `MGLOfflinePack` objects in the
 `packs` property change, even if the underlying data for these packs has not
 changed. If this method is called while a pack is actively downloading, the
 behavior is undefined.
 
 You typically do not need to call this method.
 
 To detect when the shared offline storage object has finished reloading its
 `packs` property, observe KVO change notifications on the `packs` key path.
 A reload results in an `NSKeyValueChangeSetting` change.
 */
- (void)reloadPacks;

/**
 Sets the maximum number of Mapbox-hosted tiles that may be downloaded and
 stored on the current device.
 
 Once this limit is reached, an
 `MGLOfflinePackMaximumMapboxTilesReachedNotification` is posted for every
 attempt to download additional tiles until already downloaded tiles are removed
 by calling the `-removePack:withCompletionHandler:` method.
 
 @note The <a href="https://www.mapbox.com/tos/">Mapbox Terms of Service</a>
    prohibits changing or bypassing this limit without permission from Mapbox.
    Contact your Mapbox sales representative to have the limit raised.
 */
- (void)setMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

/**
 The cumulative size, measured in bytes, of all downloaded resources on disk.
 
 The returned value includes all resources, including tiles, whether downloaded
 as part of an offline pack or due to caching during normal use of `MGLMapView`.
 */
@property (nonatomic, readonly) unsigned long long countOfBytesCompleted;

@end

NS_ASSUME_NONNULL_END
