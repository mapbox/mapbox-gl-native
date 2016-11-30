#include <mbgl/style/cross_faded_property_evaluator.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>

namespace mbgl {
namespace style {

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const Undefined&) const {
    return calculate(defaultValue, defaultValue, defaultValue);
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const T& constant) const {
    return calculate(constant, constant, constant);
}

template <typename T>
T getBiggestStopLessThan(const Function<T>& function, float z) {
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
