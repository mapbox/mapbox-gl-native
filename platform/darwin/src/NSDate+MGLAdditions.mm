#import "NSDate+MGLAdditions.h"

mbgl::Duration MGLDurationFromTimeInterval(NSTimeInterval duration)
{
    return std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<NSTimeInterval>(duration));
}

NSTimeInterval MGLTimeIntervalFromDuration(mbgl::Duration duration)
{
    std::chrono::nanoseconds nano(duration.count());
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(nano);
    
    return sec.count();
}
