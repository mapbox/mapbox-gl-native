#ifndef LLMR_STYLE_FUNCTION_PROPERTIES
#define LLMR_STYLE_FUNCTION_PROPERTIES

#include <vector>

namespace llmr {

namespace functions {

float null(float z, const std::vector<float>&);
float constant(float z, const std::vector<float>& values);
float min(float z, const std::vector<float>& values);
float max(float z, const std::vector<float>& values);
float stops(float z, const std::vector<float>& values);
float linear(float z, const std::vector<float>& values);
float exponential(float z, const std::vector<float>& values);

}

struct FunctionProperty {
    typedef float (*fn)(float z, const std::vector<float>& values);

    fn function;
    std::vector<float> values;

    inline FunctionProperty() : function(&functions::null) {}

    inline FunctionProperty(const FunctionProperty &property)
        : function(property.function), values(property.values) {}
    inline FunctionProperty(FunctionProperty &&property)
        : function(property.function), values(std::move(property.values)) {}


    inline void operator=(const FunctionProperty &rhs) {
        function = rhs.function;
        values = rhs.values;
    }

    inline FunctionProperty(float value) : function(&functions::constant), values(1, value) {}
    template <typename T> inline T evaluate(float z) const { return function(z, values); }
};

}

#endif
