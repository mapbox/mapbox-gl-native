#import "NSDate+MGLAdditions.h"

mbgl::Duration MGLDurationFromTimeInterval(NSTimeInterval duration)
{
    return std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<NSTimeInterval>(duration));
}

NSTimeInterval MGLTimeIntervalFromDuration(mbgl::Duration duration)
{
    return std::chrono::duration<NSTimeInterval, std::ratio<1>>(duration).count();
}
