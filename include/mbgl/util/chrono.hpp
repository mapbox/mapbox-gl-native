#pragma once

#include <chrono>
#include <string>

namespace mbgl {

using Clock = std::chrono::steady_clock;

using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;

using TimePoint = Clock::time_point;
using Duration  = Clock::duration;

// Used to measure second-precision times, such as times gathered from HTTP responses.
using Timestamp = std::chrono::time_point<std::chrono::system_clock, Seconds>;

namespace util {

inline Timestamp now() {
    return std::chrono::time_point_cast<Seconds>(std::chrono::system_clock::now());
}

// Returns the RFC1123 formatted date. E.g. "Tue, 04 Nov 2014 02:13:24 GMT"
std::string rfc1123(Timestamp);

// YYYY-mm-dd HH:MM:SS e.g. "2015-11-26 16:11:23"
std::string iso8601(Timestamp);

Timestamp parseTimestamp(const char *);

Timestamp parseTimestamp(const int32_t timestamp);

// C++17 polyfill
template <class Rep, class Period, class = std::enable_if_t<
   std::chrono::duration<Rep, Period>::min() < std::chrono::duration<Rep, Period>::zero()>>
constexpr std::chrono::duration<Rep, Period> abs(std::chrono::duration<Rep, Period> d)
{
    return d >= d.zero() ? d : -d;
}

} // namespace util

} // namespace mbgl
