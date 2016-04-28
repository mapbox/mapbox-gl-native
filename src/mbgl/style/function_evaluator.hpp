#ifndef MBGL_STYLE_FUNCTION_EVALUATOR
#define MBGL_STYLE_FUNCTION_EVALUATOR

#include <mbgl/style/function.hpp>
#include <mbgl/util/interpolate.hpp>

namespace mbgl {

class StyleCalculationParameters;

template <typename T>
class NormalFunctionEvaluator {
public:
    using ResultType = T;
    T operator()(const Function<T>&, const StyleCalculationParameters&) const;
};

template <typename T>
struct Faded {
    Faded() = default;
    Faded(const T& v) : to(v) {}

    T from;
    float fromScale = 0;
    T to;
    float toScale = 0;
    float t = 0;
};

template <typename T>
class CrossFadedFunctionEvaluator {
public:
    using ResultType = Faded<T>;
    Faded<T> operator()(const Function<T>&, const StyleCalculationParameters&) const;
};

namespace util {
template <typename T>
struct Interpolator<Faded<T>>
    : Uninterpolated {};
}

} // namespace mbgl

#endif
