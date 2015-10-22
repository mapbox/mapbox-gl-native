#ifndef MBGL_STYLE_FADEDFUNCTION_PROPERTIES
#define MBGL_STYLE_FADEDFUNCTION_PROPERTIES

#include <mbgl/style/style_calculation_parameters.hpp>
#include <mbgl/util/chrono.hpp>

#include <mapbox/optional.hpp>

#include <vector>

namespace mbgl {

template <typename T>
struct PiecewiseConstantFunction {
    PiecewiseConstantFunction(const std::vector<std::pair<float, T>>& values_,
                              mapbox::util::optional<Duration> duration_)
        : values(values_),
          duration(duration_) {
    }

    PiecewiseConstantFunction(T& value, mapbox::util::optional<Duration> duration_)
        : values({{ 0, value }}),
          duration(duration_) {
    }

    T evaluate(const StyleCalculationParameters&) const;

private:
    const std::vector<std::pair<float, T>> values;
    const mapbox::util::optional<Duration> duration;
};

}

#endif
