#import <Foundation/Foundation.h>
#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

MGL_EXPORT
@interface MGLMapboxEventsLogging : NSObject

+ (void)displayLogFileFromDate:(NSDate *)logDate;

@end

NS_ASSUME_NONNULL_END
