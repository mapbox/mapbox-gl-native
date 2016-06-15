#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/calculation_parameters.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/color.hpp>

#include <cmath>

namespace mbgl {
namespace style {

template <typename T>
inline T defaultStopsValue();

template <> inline bool defaultStopsValue() { return true; }
template <> inline float defaultStopsValue() { return 1.0f; }
template <> inline Color defaultStopsValue() { return { 0, 0, 0, 1 }; }
template <> inline std::vector<float> defaultStopsValue() { return {{ 1, 0 }}; }
template <> inline std::vector<std::string> defaultStopsValue() { return {{}}; }
template <> inline std::array<float, 2> defaultStopsValue() { return {{ 0, 0 }}; }

template <> inline std::string defaultStopsValue() { return {}; }
template <> inline TranslateAnchorType defaultStopsValue() { return {}; }
template <> inline RotateAnchorType defaultStopsValue() { return {}; }
template <> inline LineCapType defaultStopsValue() { return {}; }
template <> inline LineJoinType defaultStopsValue() { return {}; }
template <> inline SymbolPlacementType defaultStopsValue() { return {}; }
template <> inline TextAnchorType defaultStopsValue() { return {}; }
template <> inline TextJustifyType defaultStopsValue() { return {}; }
template <> inline TextTransformType defaultStopsValue() { return {}; }
template <> inline AlignmentType defaultStopsValue() { return {}; }

template <typename T>
T PropertyEvaluator<T>::operator()(const Function<T>& fn) const {
    float base = fn.getBase();
    const std::vector<std::pair<float, T>>& stops = fn.getStops();
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

template class PropertyEvaluator<bool>;
template class PropertyEvaluator<float>;
template class PropertyEvaluator<Color>;
template class PropertyEvaluator<std::vector<float>>;
template class PropertyEvaluator<std::vector<std::string>>;
template class PropertyEvaluator<std::array<float, 2>>;

template class PropertyEvaluator<std::string>;
template class PropertyEvaluator<TranslateAnchorType>;
template class PropertyEvaluator<RotateAnchorType>;
template class PropertyEvaluator<LineCapType>;
template class PropertyEvaluator<LineJoinType>;
template class PropertyEvaluator<SymbolPlacementType>;
template class PropertyEvaluator<TextAnchorType>;
template class PropertyEvaluator<TextJustifyType>;
template class PropertyEvaluator<TextTransformType>;
template class PropertyEvaluator<AlignmentType>;

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const Undefined&) const {
    return calculate(defaultValue, defaultValue, defaultValue);
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const T& constant) const {
    return calculate(constant, constant, constant);
}

template <typename T>
inline T getBiggestStopLessThan(const Function<T>& function, float z) {
    const auto& stops = function.getStops();
    for (uint32_t i = 0; i < stops.size(); i++) {
        if (stops[i].first > z) {
            return stops[i == 0 ? i : i - 1].second;
        }
    }
    return stops.at(stops.size() - 1).second;
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const Function<T>& function) const {
    return calculate(getBiggestStopLessThan(function, parameters.z - 1.0f),
                     getBiggestStopLessThan(function, parameters.z),
                     getBiggestStopLessThan(function, parameters.z + 1.0f));
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::calculate(const T& min, const T& mid, const T& max) const {
    const float z = parameters.z;
    const float fraction = z - std::floor(z);
    const std::chrono::duration<float> d = parameters.defaultFadeDuration;
    const float t = std::min((parameters.now - parameters.zoomHistory.lastIntegerZoomTime) / d, 1.0f);

    return z > parameters.zoomHistory.lastIntegerZoom
        ? Faded<T> { min, mid, 2.0f, 1.0f, fraction + (1.0f - fraction) * t }
        : Faded<T> { max, mid, 0.5f, 1.0f, 1 - (1 - t) * fraction };
}

template class CrossFadedPropertyEvaluator<std::string>;
template class CrossFadedPropertyEvaluator<std::vector<float>>;

} // namespace style
} // namespace mbgl
