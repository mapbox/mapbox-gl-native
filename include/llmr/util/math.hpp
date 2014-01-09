#ifndef LLMR_UTIL_MATH
#define LLMR_UTIL_MATH

#include <cmath>

namespace llmr {
namespace util {

inline double angle_between(double ax, double ay, double bx, double by) {
    return atan2((ax * by - ay * bx), ax * bx + ay * by);
}

}
}

#endif
