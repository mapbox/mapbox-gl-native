#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLOfflineTask;
@protocol MGLOfflineRegion;

typedef void (^MGLOfflineTaskRegistrationCompletionHandler)(MGLOfflineTask *task, NSError *error);
typedef void (^MGLOfflineTaskRemovalCompletionHandler)(NSError *error);
typedef void (^MGLOfflineTasksRetrievalCompletionHandler)(NS_ARRAY_OF(MGLOfflineTask *) *tasks, NSError *error);

@interface MGLOfflineStorage : NSObject

+ (instancetype)sharedOfflineStorage;

- (instancetype)init NS_UNAVAILABLE;

- (void)addTaskForRegion:(id <MGLOfflineRegion>)region withContext:(NSData *)context completionHandler:(MGLOfflineTaskRegistrationCompletionHandler)completion;

- (void)removeTask:(MGLOfflineTask *)task withCompletionHandler:(MGLOfflineTaskRemovalCompletionHandler)completion;

- (void)getTasksWithCompletionHandler:(MGLOfflineTasksRetrievalCompletionHandler)completion;

- (void)setMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
