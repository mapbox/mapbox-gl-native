#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/style/expression/image.hpp>
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
    return z > parameters.zoomHistory.lastIntegerZoom
        ? Faded<T> { min, mid }
        : Faded<T> { max, mid };
}

template class CrossFadedPropertyEvaluator<style::expression::Image>;
template class CrossFadedPropertyEvaluator<std::vector<float>>;

} // namespace mbgl
