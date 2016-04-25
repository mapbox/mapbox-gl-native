#pragma once

#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/function_evaluator.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <utility>

namespace mbgl {

template <typename T>
class LayoutProperty {
public:
    explicit LayoutProperty(T v) : value(std::move(v)) {}

    void parse(const char * name, const JSValue& layout) {
        if (layout.HasMember(name)) {
            parsedValue = parseProperty<T>(name, layout[name]);
        }
    }

    void calculate(const StyleCalculationParameters& parameters) {
        if (parsedValue) {
            NormalFunctionEvaluator<T> evaluator;
            value = evaluator(*parsedValue, parameters);
        }
    }

    operator T() const { return value; }

    optional<Function<T>> parsedValue;
    T value;
};

} // namespace mbgl
