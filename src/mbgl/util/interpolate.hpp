#ifndef MBGL_UTIL_INTERPOLATE
#define MBGL_UTIL_INTERPOLATE

#include <array>
#include <vector>

#include <mbgl/style/types.hpp>

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

template <typename T>
inline std::array<T, 2> interpolate(const std::array<T, 2>& a, const std::array<T, 2>& b, const double t) {
    return {{
        interpolate(a[0], b[0], t),
        interpolate(a[1], b[1], t)
    }};
}

// fake interpolations that just return the first value
template<> inline bool interpolate(const bool a, const bool, const double) { return a; }
template<> inline std::vector<float> interpolate(const std::vector<float> a, const std::vector<float>, const double) { return a; }
template<> inline std::string interpolate(const std::string a, const std::string, const double) { return a; }
template<> inline TranslateAnchorType interpolate(const TranslateAnchorType a, const TranslateAnchorType, const double) { return a; }
template<> inline RotateAnchorType interpolate(const RotateAnchorType a, const RotateAnchorType, const double) { return a; }
template<> inline CapType interpolate(const CapType a, const CapType, const double) { return a; }
template<> inline JoinType interpolate(const JoinType a, const JoinType, const double) { return a; }
template<> inline PlacementType interpolate(const PlacementType a, const PlacementType, const double) { return a; }
template<> inline TextAnchorType interpolate(const TextAnchorType a, const TextAnchorType, const double) { return a; }
template<> inline TextJustifyType interpolate(const TextJustifyType a, const TextJustifyType, const double) { return a; }
template<> inline TextTransformType interpolate(const TextTransformType a, const TextTransformType, const double) { return a; }
template<> inline RotationAlignmentType interpolate(const RotationAlignmentType a, const RotationAlignmentType, const double) { return a; }

template<> inline Faded<std::string> interpolate(const Faded<std::string>, const Faded<std::string> b, const double) { return b; }
template<> inline Faded<std::vector<float>> interpolate(const Faded<std::vector<float>>, const Faded<std::vector<float>> b, const double) { return b; }

}
}

#endif
