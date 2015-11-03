#include <mbgl/style/function.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>
#include <mbgl/util/interpolate.hpp>

#include <cmath>

namespace mbgl {

template <typename T>
inline T defaultStopsValue();

template <> inline bool defaultStopsValue() { return true; }
template <> inline float defaultStopsValue() { return 1.0f; }
template <> inline Color defaultStopsValue() { return {{ 0, 0, 0, 1 }}; }
template <> inline std::vector<float> defaultStopsValue() { return {{ 1, 0 }}; }
template <> inline std::array<float, 2> defaultStopsValue() { return {{ 0, 0 }}; }

template <> inline std:: string defaultStopsValue() { return {}; }
template <> inline TranslateAnchorType defaultStopsValue() { return {}; };
template <> inline RotateAnchorType defaultStopsValue() { return {}; };
template <> inline CapType defaultStopsValue() { return {}; };
template <> inline JoinType defaultStopsValue() { return {}; };
template <> inline PlacementType defaultStopsValue() { return {}; };
template <> inline TextAnchorType defaultStopsValue() { return {}; };
template <> inline TextJustifyType defaultStopsValue() { return {}; };
template <> inline TextTransformType defaultStopsValue() { return {}; };
template <> inline RotationAlignmentType defaultStopsValue() { return {}; };

template <typename T>
T Function<T>::evaluate(const StyleCalculationParameters& parameters) const {
    float z = parameters.z;
    bool smaller = false;
    float smaller_z = 0.0f;
    T smaller_val = T();
    bool larger = false;
    float larger_z = 0.0f;
    T larger_val = T();

    for (uint32_t i = 0; i < stops.size(); i++) {
        float stop_z = stops[i].first;
        T stop_val = stops[i].second;
        if (stop_z <= z && (!smaller || smaller_z < stop_z)) {
            smaller = true;
            smaller_z = stop_z;
            smaller_val = stop_val;
        }
        if (stop_z >= z && (!larger || larger_z > stop_z)) {
            larger = true;
            larger_z = stop_z;
            larger_val = stop_val;
        }
    }

    if (smaller && larger) {
        if (larger_z == smaller_z || larger_val == smaller_val) {
            return smaller_val;
        }
        const float zoomDiff = larger_z - smaller_z;
        const float zoomProgress = z - smaller_z;
        if (base == 1.0f) {
            const float t = zoomProgress / zoomDiff;
            return util::interpolate(smaller_val, larger_val, t);
        } else {
            const float t = (std::pow(base, zoomProgress) - 1) / (std::pow(base, zoomDiff) - 1);
            return util::interpolate(smaller_val, larger_val, t);
        }
    } else if (larger) {
        return larger_val;
    } else if (smaller) {
        return smaller_val;
    } else {
        // No stop defined.
        return defaultStopsValue<T>();
    }
}

template class Function<bool>;
template class Function<float>;
template class Function<Color>;
template class Function<std::vector<float>>;
template class Function<std::array<float, 2>>;

template class Function<std::string>;
template class Function<TranslateAnchorType>;
template class Function<RotateAnchorType>;
template class Function<CapType>;
template class Function<JoinType>;
template class Function<PlacementType>;
template class Function<TextAnchorType>;
template class Function<TextJustifyType>;
template class Function<TextTransformType>;
template class Function<RotationAlignmentType>;

template <typename T>
size_t getBiggestStopLessThan(std::vector<std::pair<float, T>> stops, float z) {
    for (uint32_t i = 0; i < stops.size(); i++) {
        if (stops[i].first > z) {
            return i == 0 ? i : i - 1;
        }
    }
    return stops.size() - 1;
}

template <typename T>
Faded<T> Function<Faded<T>>::evaluate(const StyleCalculationParameters& parameters) const {
    Faded<T> result;

    float z = parameters.z;
    float fraction = std::fmod(z, 1.0f);
    std::chrono::duration<float> d = duration ? *duration : parameters.defaultFadeDuration;
    float t = std::min((Clock::now() - parameters.zoomHistory.lastIntegerZoomTime) / d, 1.0f);
    float fromScale = 1.0f;
    float toScale = 1.0f;
    size_t from, to;

    if (z > parameters.zoomHistory.lastIntegerZoom) {
        result.t = fraction + (1.0f - fraction) * t;
        from = getBiggestStopLessThan(stops, z - 1.0f);
        to = getBiggestStopLessThan(stops, z);
        fromScale *= 2.0f;

    } else {
        result.t = 1 - (1 - t) * fraction;
        to = getBiggestStopLessThan(stops, z);
        from = getBiggestStopLessThan(stops, z + 1.0f);
        fromScale /= 2.0f;
    }

    result.from = stops[from].second;
    result.to = stops[to].second;
    result.fromScale = fromScale;
    result.toScale = toScale;
    return result;
}

template class Function<Faded<std::string>>;
template class Function<Faded<std::vector<float>>>;

}
