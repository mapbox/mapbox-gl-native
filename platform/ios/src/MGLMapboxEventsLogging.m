#import "MGLMapboxEventsLogging.h"
#import "MGLMapboxEvents.h"

@implementation MGLMapboxEventsLogging

+ (void)displayLogFileFromDate:(NSDate *)logDate {
    [MGLMapboxEvents displayLogFileFromDate:logDate];
}

@end
