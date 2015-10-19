#ifndef MBGL_PROPERTY_EVALUATOR
#define MBGL_PROPERTY_EVALUATOR

#include <mbgl/style/zoom_history.hpp>
#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/piecewisefunction_properties.hpp>

namespace mbgl {

struct ZoomHistory;

template <typename T>
class PropertyEvaluator {
public:
    typedef T result_type;

    PropertyEvaluator(float z_) : z(z_), zoomHistory() {}
    PropertyEvaluator(float z_, const ZoomHistory& zoomHistory_) : z(z_), zoomHistory(zoomHistory_) {}

    template <typename P, typename std::enable_if<std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &value) const {
        return value;
    }

    T operator()(const Function<T>& value) const {
        return mapbox::util::apply_visitor(FunctionEvaluator<T>(z), value);
    }

    T operator()(const PiecewiseConstantFunction<T>& value) const {
        return value.evaluate(z, zoomHistory);
    }

    template <typename P, typename std::enable_if<!std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &) const {
        return T();
    }

private:
    const float z;
    ZoomHistory zoomHistory;
};

}

#endif
