#include <mbgl/util/chrono.hpp>

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

} // namespace mbgl
