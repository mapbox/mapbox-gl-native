#ifndef LLMR_UTIL_TIME
#define LLMR_UTIL_TIME

#include <uv.h>

namespace llmr {

typedef uint64_t time;

namespace util {


// Returns time in nanoseconds since an arbitrary point in the past. This has
// no relation to the current time, but is continuously increasing, so
// comparisons between different timestamps produce meaningful values.
inline time now() {
    return uv_hrtime();
}
}

// used for time conversions
constexpr time operator"" _nanoseconds (long double ns) { return ns; }
constexpr time operator"" _nanosecond (long double ns) { return ns; }
constexpr time operator"" _microseconds (long double us) { return us * 1e3; }
constexpr time operator"" _microsecond (long double us) { return us * 1e3; }
constexpr time operator"" _milliseconds (long double ms) { return ms * 1e6; }
constexpr time operator"" _millisecond (long double ms) { return ms * 1e6; }
constexpr time operator"" _seconds (long double s) { return s * 1e9; }
constexpr time operator"" _second (long double s) { return s * 1e9; }

constexpr time operator"" _nanoseconds (unsigned long long ns) { return ns; }
constexpr time operator"" _nanosecond (unsigned long long ns) { return ns; }
constexpr time operator"" _microseconds (unsigned long long us) { return us * 1e3; }
constexpr time operator"" _microsecond (unsigned long long us) { return us * 1e3; }
constexpr time operator"" _milliseconds (unsigned long long ms) { return ms * 1e6; }
constexpr time operator"" _millisecond (unsigned long long ms) { return ms * 1e6; }
constexpr time operator"" _seconds (unsigned long long s) { return s * 1e9; }
constexpr time operator"" _second (unsigned long long s) { return s * 1e9; }

}

#endif
