#include <mbgl/style/fadedfunction_properties.hpp>
#include <mbgl/style/types.hpp>

#include <cmath>

namespace mbgl {

template <typename T>
uint32_t getBiggestStopLessThan(std::vector<std::pair<float, T>> stops, float z) {
    for (uint32_t i = 0; i < stops.size(); i++) {
        if (stops[i].first > z) {
            return i == 0 ? i : i - 1;
        }
    }
    return stops.size() - 1;
}

template <typename T>
T FadedStopsFunction<T>::evaluate(float z, const ZoomHistory &zh) const {
    T result;

    float fraction = std::fmod(z, 1.0f);
    float tDiff = (util::now() - zh.lastIntegerZoomTime) / 1_millisecond;
    float fDuration = duration / 1_millisecond;
    float t = std::min(tDiff / fDuration, 1.0f);
    float scale = 1.0f;
    uint32_t low, high;

    if (z > zh.lastIntegerZoom) {
        result.t = fraction + (1.0f - fraction) * t;
        scale *= 2.0f;
        low = getBiggestStopLessThan(values, z - 1.0f);
        high = getBiggestStopLessThan(values, z);

    } else {
        result.t = fraction - fraction * t;
        low = getBiggestStopLessThan(values, z);
        high = getBiggestStopLessThan(values, z + 1.0f);
    }


    result.low = values[low].second.low;
    result.high = values[high].second.low;
    result.lowScale = scale;
    result.highScale = scale / 2;
    return result;
}

template Faded<std::string> FadedStopsFunction<Faded<std::string>>::evaluate(float z, const ZoomHistory &zoomHistory) const;

}
