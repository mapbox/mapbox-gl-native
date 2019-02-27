#import <Foundation/Foundation.h>
#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

typedef void (^MGLEventHandler)(NSString *eventName, NSDictionary *attributes);

MGL_EXPORT
@interface MGLEventsManager : NSObject

/**
 Returns the shared logging object.
 */
@property (class, nonatomic, readonly) MGLEventsManager *sharedManager;

@property (nonatomic, copy, nullable) MGLEventHandler handler;

@property (nonatomic, assign) BOOL shouldHandleEvents;

@end

NS_ASSUME_NONNULL_END
