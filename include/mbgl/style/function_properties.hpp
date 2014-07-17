#ifndef MBGL_STYLE_FUNCTION_PROPERTIES
#define MBGL_STYLE_FUNCTION_PROPERTIES

#include <mbgl/util/variant.hpp>

#include <vector>

namespace mbgl {

template <typename T>
struct ConstantFunction {
    inline ConstantFunction(const T &value) : value(value) {}
    inline T evaluate(float) const { return value; }

private:
    const T value;
};

template <typename T>
struct LinearFunction {
    inline LinearFunction(const T &value, float z_base, float slope, const T &min, const T &max)
        : value(value), min(min), max(max), z_base(z_base), slope(slope) {}
    T evaluate(float z) const;

private:
    const T value, min, max;
    const float z_base, slope;
};

template <typename T>
struct ExponentialFunction {
    inline ExponentialFunction(const T &value, float z_base, float exp_base, float slope, const T &min,
                               const T &max)
        : value(value), min(min), max(max), z_base(z_base), exp_base(exp_base), slope(slope) {}
    T evaluate(float z) const;

private:
    const T value, min, max;
    const float z_base, exp_base, slope;
};

template <typename T>
struct StopsFunction {
    inline StopsFunction(const std::vector<std::pair<float, T>> &values) : values(values) {}
    T evaluate(float z) const;

private:
    const std::vector<std::pair<float, T>> values;
};

template <typename T>
using Function = util::variant<
    std::false_type,
    ConstantFunction<T>,
    LinearFunction<T>,
    ExponentialFunction<T>,
    StopsFunction<T>
>;

template <typename T>
struct FunctionEvaluator {
    typedef T result_type;
    inline FunctionEvaluator(float z) : z(z) {}

    inline result_type operator()(const std::false_type &) {
        return result_type();
    }

    template <template <typename> class Fn>
    inline result_type operator()(const Fn<T>& fn) {
        return fn.evaluate(z);
    }
private:
    float z;
};

}

#endif
