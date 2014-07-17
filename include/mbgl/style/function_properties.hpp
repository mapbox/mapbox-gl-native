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
struct StopsFunction {
    inline StopsFunction(const std::vector<std::pair<float, T>> &values, float base) : values(values), base(base) {}
    T evaluate(float z) const;

private:
    const std::vector<std::pair<float, T>> values;
    const float base;
};

template <typename T>
using Function = util::variant<
    std::false_type,
    ConstantFunction<T>,
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
