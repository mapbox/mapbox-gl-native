#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/util/interpolate.hpp>

namespace mbgl {
namespace style {

class PropertyEvaluationParameters;

template <typename T>
class PropertyEvaluator {
public:
    using ResultType = T;

    PropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    T operator()(const Undefined&) const { return defaultValue; }
    T operator()(const T& constant) const { return constant; }
    T operator()(const Function<T>&) const;

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

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

    Faded<T> operator()(const Undefined&) const;
    Faded<T> operator()(const T& constant) const;
    Faded<T> operator()(const Function<T>&) const;

private:
    Faded<T> calculate(const T& min, const T& mid, const T& max) const;

    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace style

namespace util {
template <typename T>
struct Interpolator<style::Faded<T>>
    : Uninterpolated {};
} // namespace util

} // namespace mbgl
