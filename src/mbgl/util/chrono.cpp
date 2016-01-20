#include <mbgl/util/chrono.hpp>

#include <parsedate/parsedate.h>

#include <cstdio>

namespace mbgl {

template Duration toDuration<Clock, Duration>(TimePoint);
template SystemDuration toDuration<SystemClock, SystemDuration>(SystemTimePoint);

template Seconds asSeconds<Duration>(Duration);
template Seconds asSeconds<Milliseconds>(Milliseconds);

template Seconds toSeconds<Clock, Duration>(TimePoint);
template Seconds toSeconds<SystemClock, SystemDuration>(SystemTimePoint);

template Milliseconds asMilliseconds<Duration>(Duration);
template Milliseconds asMilliseconds<Seconds>(Seconds);

template Milliseconds toMilliseconds<Clock, Duration>(TimePoint);
template Milliseconds toMilliseconds<SystemClock, SystemDuration>(SystemTimePoint);

namespace util {

static const char *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

std::string rfc1123(SystemTimePoint timePoint) {
    std::time_t time = SystemClock::to_time_t(timePoint);
    std::tm info;
    gmtime_r(&time, &info);
    char buffer[30];
    snprintf(buffer, 30, "%s, %02d %s %4d %02d:%02d:%02d GMT", week[info.tm_wday], info.tm_mday,
             months[info.tm_mon], 1900 + info.tm_year, info.tm_hour, info.tm_min, info.tm_sec);
    return buffer;
}

std::string iso8601(SystemTimePoint timePoint) {
    std::time_t time = SystemClock::to_time_t(timePoint);
    std::tm info;
    gmtime_r(&time, &info);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%F %T", &info);
    return buffer;
}

SystemTimePoint parseTimePoint(const char * timePoint) {
    return SystemClock::from_time_t(parse_date(timePoint));
}

} // namespace util

} // namespace mbgl
