#ifndef LLMR_UTIL_MATH
#define LLMR_UTIL_MATH

#include <cmath>

#include "vec.hpp"

namespace llmr {
namespace util {

inline double angle_between(double ax, double ay, double bx, double by) {
    return atan2((ax * by - ay * bx), ax * bx + ay * by);
}

template <typename T, typename S1, typename S2>
inline vec2<T> normal(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = sqrt(dx * dx + dy * dy);
    return { dx / c, dy / c };
}

template <typename T, typename S1, typename S2>
inline T dist(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = sqrt(dx * dx + dy * dy);
    return c;
}

}
}

#endif
