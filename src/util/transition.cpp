#include <mbgl/util/transition.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/platform/platform.hpp>

namespace mbgl { namespace util {

UnitBezier ease(0, 0, 0.25, 1);

transition::~transition() {}

double transition::interpolateDouble(double from, double to, double t) const {
    return from + (to - from) * t;
}

float transition::interpolateFloat(float from, float to, double t) const {
    return from + (to - from) * (float)t;
}

mbgl::Color transition::interpolateColor(mbgl::Color from, mbgl::Color to, double t) const {
    return {{ interpolateFloat(from[0], to[0], t),
              interpolateFloat(from[1], to[1], t),
              interpolateFloat(from[2], to[2], t),
              interpolateFloat(from[3], to[3], t) }};
}

std::array<float, 2> transition::interpolateFloatArray(std::array<float, 2> from, std::array<float, 2> to, double t) const {
    return {{ interpolateFloat(from[0], to[0], t), interpolateFloat(from[1], to[1], t) }};
}

template <>
transition::state ease_transition<double>::update(timestamp now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = interpolateDouble(from, to, ease.solve(t, 0.001));
        return running;
    }
}

template <>
transition::state ease_transition<mbgl::Color>::update(timestamp now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = interpolateColor(from, to, ease.solve(t, 0.001));
        return running;
    }
}

template <>
transition::state ease_transition<float>::update(timestamp now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = interpolateFloat(from, to, ease.solve(t, 0.001));
        return running;
    }
}

template <>
transition::state ease_transition<std::array<float, 2>>::update(timestamp now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = interpolateFloatArray(from, to, ease.solve(t, 0.001));
        return running;
    }
}

}}
