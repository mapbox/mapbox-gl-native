#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/util/interpolate.hpp>

namespace mbgl {

template <typename T>
class Faded {
public:
    T from;
    T to;
    float fromScale;
    float toScale;
    float t;
};

template <typename T>
class CrossFadedPropertyEvaluator {
public:
    using ResultType = Faded<T>;

    CrossFadedPropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    Faded<T> operator()(const style::Undefined&) const;
    Faded<T> operator()(const T& constant) const;
    Faded<T> operator()(const style::CameraFunction<T>&) const;

private:
    Faded<T> calculate(const T& min, const T& mid, const T& max) const;

    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

namespace util {
template <typename T>
struct Interpolator<Faded<T>>
    : Uninterpolated {};
} // namespace util

} // namespace mbgl
