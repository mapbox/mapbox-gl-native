#ifndef MBGL_STYLE_FUNCTION_PROPERTIES
#define MBGL_STYLE_FUNCTION_PROPERTIES

#include <mapbox/variant.hpp>

#include <vector>

namespace mbgl {

template <typename T>
struct ConstantFunction {
    inline ConstantFunction(const T &value_) : value(value_) {}
    inline T evaluate(float) const { return value; }

private:
    const T value;
};

template <typename T>
struct StopsFunction {
    inline StopsFunction(const std::vector<std::pair<float, T>> &values_, float base_) : values(values_), base(base_) {}
    T evaluate(float z) const;

private:
    const std::vector<std::pair<float, T>> values;
    const float base;
};

template <typename T>
using Function = mapbox::util::variant<
    std::false_type,
    ConstantFunction<T>,
    StopsFunction<T>
>;

template <typename T>
struct FunctionEvaluator {
    typedef T result_type;
    inline FunctionEvaluator(float z_) : z(z_) {}

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
