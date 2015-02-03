#ifndef MBGL_STYLE_FADEDFUNCTION_PROPERTIES
#define MBGL_STYLE_FADEDFUNCTION_PROPERTIES

#include <mbgl/style/zoom_history.hpp>

#include <vector>

namespace mbgl {

template <typename T>
struct FadedStopsFunction {
    inline FadedStopsFunction(const std::vector<std::pair<float, T>> &values_, timestamp duration_) : values(values_), duration(duration_) {}
    inline FadedStopsFunction(T &value) : values({{ 0, value }}), duration(300_milliseconds) {}
    inline FadedStopsFunction() : values(), duration(300_milliseconds) {}
    T evaluate(float z, const ZoomHistory &zoomHistory) const;

private:
    const std::vector<std::pair<float, T>> values;
    const timestamp duration;
};

}

#endif
