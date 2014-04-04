#ifndef LLMR_UTIL_TIME
#define LLMR_UTIL_TIME

namespace llmr {
namespace util {

// Returns time in milliseconds since an arbitrary point in the past. This has
// no relation to the current time, but is continuously increasing, so
// comparisons between different timestamps produce meaningful values.
float time();


}
// used for time conversions
constexpr float operator"" _nanoseconds (long double ns) { return ns / 1e6; }
constexpr float operator"" _microseconds (long double us) { return us / 1e3; }
constexpr float operator"" _milliseconds (long double ms) { return ms; }
constexpr float operator"" _seconds (long double s) { return s * 1e3; }

constexpr float operator"" _nanoseconds (unsigned long long ns) { return (double)ns / 1e6; }
constexpr float operator"" _microseconds (unsigned long long us) { return (double)us / 1e3; }
constexpr float operator"" _milliseconds (unsigned long long ms) { return ms; }
constexpr float operator"" _seconds (unsigned long long s) { return s * 1e3; }

}

#endif