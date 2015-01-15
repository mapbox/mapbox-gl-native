#ifndef MBGL_UTIL_INTERPOLATE
#define MBGL_UTIL_INTERPOLATE

#include <array>
#include <vector>

namespace mbgl {
namespace util {

template <typename T>
T interpolate(const T a, const T b, const double t) {
    return a * (1.0 - t) + b * t;
}

template <typename T>
inline std::array<T, 4> interpolate(const std::array<T, 4>& a, const std::array<T, 4>& b, const double t) {
    return {{
        interpolate(a[0], b[0], t),
        interpolate(a[1], b[1], t),
        interpolate(a[2], b[2], t),
        interpolate(a[3], b[3], t)
    }};
}

inline std::vector<float> interpolate(const std::vector<float> &a, const std::vector<float> &b, const double t) {
    fprintf(stderr, "interpolate this\n");
    return a;
}

}
}

#endif
