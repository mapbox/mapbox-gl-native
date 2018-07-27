#import <Foundation/Foundation.h>
#import "MGLMapboxEvents.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLMapboxEventsLogging : NSObject

+ (void)displayLogFileFromDate:(NSDate *)logDate;

@end

NS_ASSUME_NONNULL_END
