#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/util/interpolate.hpp>

namespace mbgl {

class StyleCalculationParameters;

template <typename T>
class PropertyEvaluator {
public:
    using ResultType = T;

    PropertyEvaluator(const StyleCalculationParameters& parameters_, const T& defaultValue_)
        : parameters(parameters_),
          defaultValue(defaultValue_) {}

    T operator()(const Undefined&) const { return defaultValue; }
    T operator()(const T& constant) const { return constant; }
    T operator()(const Function<T>&) const;

private:
    const StyleCalculationParameters& parameters;
    T defaultValue;
};

template <typename T>
struct Faded {
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

    CrossFadedPropertyEvaluator(const StyleCalculationParameters& parameters_, const T& defaultValue_)
        : parameters(parameters_),
          defaultValue(defaultValue_) {}

    Faded<T> operator()(const Undefined&) const;
    Faded<T> operator()(const T& constant) const;
    Faded<T> operator()(const Function<T>&) const;

private:
    Faded<T> calculate(const T& min, const T& mid, const T& max) const;

    const StyleCalculationParameters& parameters;
    T defaultValue;
};

namespace util {
template <typename T>
struct Interpolator<Faded<T>>
    : Uninterpolated {};
}

} // namespace mbgl
