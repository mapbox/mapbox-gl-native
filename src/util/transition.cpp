#include <llmr/util/transition.hpp>
#include <llmr/util/unitbezier.hpp>
#include <llmr/platform/platform.hpp>

using namespace llmr::util;

UnitBezier ease(0.25, 0.1, 0.25, 1);

transition::~transition() {}

double transition::interpolateDouble(double from, double to, double t) const {
    return from + (to - from) * t;
}

float transition::interpolateFloat(float from, float to, double t) const {
    return from + (to - from) * (float)t;
}

llmr::Color transition::interpolateColor(llmr::Color from, llmr::Color to, double t) const {
    return {{ interpolateFloat(from[0], to[0], t),
              interpolateFloat(from[1], to[1], t),
              interpolateFloat(from[2], to[2], t),
              interpolateFloat(from[3], to[3], t) }};
}

std::vector<float> transition::interpolateFloatArray(std::vector<float> from, std::vector<float> to, double t) const {
    if (from.size() != to.size()) {
        throw std::runtime_error("unable to interpolate differently-sized arrays");
    }

    std::vector<float> return_array;
    return_array.reserve(from.size());

    for (uint16_t i = 0; i < from.size(); i++) {
        return_array.push_back(interpolateFloat(from[0], to[0], t));
    }

    return return_array;
}

template <>
transition::state ease_transition<double>::update(llmr::time now) const {
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
transition::state ease_transition<llmr::Color>::update(llmr::time now) const {
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
transition::state ease_transition<float>::update(llmr::time now) const {
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
transition::state ease_transition<std::vector<float>>::update(llmr::time now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = interpolateFloatArray(from, to, ease.solve(t, 0.001));
        return running;
    }
}
