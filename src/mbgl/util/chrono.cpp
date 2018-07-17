#include <mbgl/util/chrono.hpp>

#include <parsedate/parsedate.hpp>

#include <cstdio>
#include <ctime>

#if defined(_WINDOWS)
#define _gmtime(t, i) gmtime_s(i, t)
#else
#define _gmtime(t, i) gmtime_r(t, i)
#endif

namespace mbgl {
namespace util {

static const char *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

std::string rfc1123(Timestamp timestamp) {
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm info;
    _gmtime(&time, &info);
    char buffer[30];
    snprintf(buffer, 30, "%s, %02d %s %4d %02d:%02d:%02d GMT", week[info.tm_wday], info.tm_mday,
             months[info.tm_mon], 1900 + info.tm_year, info.tm_hour, info.tm_min, info.tm_sec);
    return buffer;
}

std::string iso8601(Timestamp timestamp) {
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm info;
    _gmtime(&time, &info);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%F %T", &info);
    return buffer;
}

Timestamp parseTimestamp(const char* timestamp) {
    return std::chrono::time_point_cast<Seconds>(std::chrono::system_clock::from_time_t(parse_date(timestamp)));
}

Timestamp parseTimestamp(const int32_t timestamp) {
    return std::chrono::time_point_cast<Seconds>(std::chrono::system_clock::from_time_t(timestamp));
}

} // namespace util

} // namespace mbgl
