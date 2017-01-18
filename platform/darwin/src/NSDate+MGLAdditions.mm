#import "NSDate+MGLAdditions.h"

@implementation NSDate (MGLAdditions)

mbgl::Duration MGLDurationInSecondsFromTimeInterval(NSTimeInterval duration)
{
    return std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<NSTimeInterval>(duration));
}

NSTimeInterval MGLTimeIntervalFromDurationInSeconds(mbgl::Duration duration)
{
    return duration.count();
}

@end
