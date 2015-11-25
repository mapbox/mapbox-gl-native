#ifndef MBGL_UTIL_CHRONO
#define MBGL_UTIL_CHRONO

#include <chrono>

namespace mbgl {

using Clock = std::chrono::steady_clock;
using SystemClock = std::chrono::system_clock;

using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;

using TimePoint = Clock::time_point;
using Duration  = Clock::duration;

using SystemTimePoint = SystemClock::time_point;
using SystemDuration = SystemClock::duration;

template <class _Clock, class _Duration>
_Duration toDuration(std::chrono::time_point<_Clock, _Duration> time_point) {
    return time_point.time_since_epoch();
}

template <class _Duration>
Seconds asSeconds(_Duration duration) {
    return std::chrono::duration_cast<Seconds>(duration);
}

template <class _Clock, class _Duration>
Seconds toSeconds(std::chrono::time_point<_Clock, _Duration> time_point) {
    return asSeconds(toDuration<_Clock, _Duration>(time_point));
}

template <class _Duration>
Milliseconds asMilliseconds(_Duration duration) {
    return std::chrono::duration_cast<Milliseconds>(duration);
}

template <class _Clock, class _Duration>
Milliseconds toMilliseconds(std::chrono::time_point<_Clock, _Duration> time_point) {
    return asMilliseconds(toDuration<_Clock, _Duration>(time_point));
}

}

#endif
