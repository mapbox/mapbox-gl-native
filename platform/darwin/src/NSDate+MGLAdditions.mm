#import "NSDate+MGLAdditions.h"

@implementation NSDate (MGLAdditions)

mbgl::Duration MGLDurationInSeconds(NSTimeInterval duration)
{
    return std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<NSTimeInterval>(duration));
}

NSTimeInterval MGLSecondsFromDuration(mbgl::Duration duration)
{
    return duration.count();
}

@end
