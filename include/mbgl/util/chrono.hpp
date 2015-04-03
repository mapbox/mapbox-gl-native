#ifndef MBGL_UTIL_CHRONO
#define MBGL_UTIL_CHRONO

#include <chrono>

namespace mbgl {

using Clock = std::chrono::steady_clock;
using SystemClock = std::chrono::system_clock;

using TimePoint = Clock::time_point;
using Duration  = Clock::duration;

}

#endif
