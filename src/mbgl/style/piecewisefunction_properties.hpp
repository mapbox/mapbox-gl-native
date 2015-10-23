#ifndef MBGL_STYLE_FADEDFUNCTION_PROPERTIES
#define MBGL_STYLE_FADEDFUNCTION_PROPERTIES

#include <mbgl/style/style_calculation_parameters.hpp>

#include <vector>

namespace mbgl {

template <typename T>
struct PiecewiseConstantFunction {
    inline PiecewiseConstantFunction(const std::vector<std::pair<float, T>> &values_, std::chrono::duration<float> duration_) : values(values_), duration(duration_) {}
    inline PiecewiseConstantFunction(T &value, std::chrono::duration<float> duration_) : values({{ 0, value }}), duration(duration_) {}

    T evaluate(const StyleCalculationParameters&) const;

private:
    const std::vector<std::pair<float, T>> values;
    const std::chrono::duration<float> duration;
};

}

#endif
