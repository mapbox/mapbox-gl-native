#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/util/chrono.hpp>

#include <cmath>

namespace mbgl {

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const style::Undefined&) const {
    return calculate(defaultValue, defaultValue, defaultValue);
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const T& constant) const {
    return calculate(constant, constant, constant);
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::operator()(const style::PropertyExpression<T>& expression) const {
    return calculate(expression.evaluate(parameters.z - 1.0f),
                     expression.evaluate(parameters.z),
                     expression.evaluate(parameters.z + 1.0f));
}

template <typename T>
Faded<T> CrossFadedPropertyEvaluator<T>::calculate(const T& min, const T& mid, const T& max) const {
    const float z = parameters.z;
    const float fraction = z - std::floor(z);
    const std::chrono::duration<float> d = parameters.defaultFadeDuration;
    const float t =
        d != std::chrono::duration<float>::zero()
            ? std::min((parameters.now - parameters.zoomHistory.lastIntegerZoomTime) / d, 1.0f)
            : 1.0f;

    return z > parameters.zoomHistory.lastIntegerZoom
        ? Faded<T> { min, mid, 2.0f, 1.0f, fraction + (1.0f - fraction) * t }
        : Faded<T> { max, mid, 0.5f, 1.0f, 1 - (1 - t) * fraction };
}

template class CrossFadedPropertyEvaluator<std::string>;
template class CrossFadedPropertyEvaluator<std::vector<float>>;

} // namespace mbgl
