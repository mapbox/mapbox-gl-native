#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLDownloadable;
@protocol MGLDownloadRegion;

typedef void (^MGLDownloadableRegistrationCompletionHandler)(MGLDownloadable *downloadable, NSError *error);
typedef void (^MGLDownloadableRemovalCompletionHandler)(NSError *error);
typedef void (^MGLDownloadablesRequestCompletionHandler)(NS_ARRAY_OF(MGLDownloadable *) *downloadables, NSError *error);

@interface MGLDownloadController : NSObject

+ (instancetype)sharedController;

- (instancetype)init NS_UNAVAILABLE;

- (void)addDownloadableForRegion:(id <MGLDownloadRegion>)downloadRegion withContext:(NSData *)context completionHandler:(MGLDownloadableRegistrationCompletionHandler)completion;

- (void)removeDownloadable:(MGLDownloadable *)downloadable withCompletionHandler:(MGLDownloadableRemovalCompletionHandler)completion;

- (void)requestDownloadablesWithCompletionHandler:(MGLDownloadablesRequestCompletionHandler)completion;

- (void)setMaximumAllowedMapboxTiles:(uint64_t)maximumCount;

@end

NS_ASSUME_NONNULL_END
