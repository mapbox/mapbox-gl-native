#ifndef MBGL_UTIL_TIME
#define MBGL_UTIL_TIME

#include <cstdint>

namespace mbgl {

typedef uint64_t timestamp;

namespace util {


// Returns time in nanoseconds since an arbitrary point in the past. This has
// no relation to the current time, but is continuously increasing, so
// comparisons between different timestamps produce meaningful values.
timestamp now();

}

// used for time conversions
constexpr timestamp operator"" _nanoseconds (long double ns) { return ns; }
constexpr timestamp operator"" _nanosecond (long double ns) { return ns; }
constexpr timestamp operator"" _microseconds (long double us) { return us * 1e3; }
constexpr timestamp operator"" _microsecond (long double us) { return us * 1e3; }
constexpr timestamp operator"" _milliseconds (long double ms) { return ms * 1e6; }
constexpr timestamp operator"" _millisecond (long double ms) { return ms * 1e6; }
constexpr timestamp operator"" _seconds (long double s) { return s * 1e9; }
constexpr timestamp operator"" _second (long double s) { return s * 1e9; }

constexpr timestamp operator"" _nanoseconds (unsigned long long ns) { return ns; }
constexpr timestamp operator"" _nanosecond (unsigned long long ns) { return ns; }
constexpr timestamp operator"" _microseconds (unsigned long long us) { return us * 1e3; }
constexpr timestamp operator"" _microsecond (unsigned long long us) { return us * 1e3; }
constexpr timestamp operator"" _milliseconds (unsigned long long ms) { return ms * 1e6; }
constexpr timestamp operator"" _millisecond (unsigned long long ms) { return ms * 1e6; }
constexpr timestamp operator"" _seconds (unsigned long long s) { return s * 1e9; }
constexpr timestamp operator"" _second (unsigned long long s) { return s * 1e9; }

}

#endif
