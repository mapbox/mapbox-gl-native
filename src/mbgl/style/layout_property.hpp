#ifndef MBGL_LAYOUT_PROPERTY
#define MBGL_LAYOUT_PROPERTY

#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/function.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <utility>

namespace mbgl {

template <typename T>
class LayoutProperty {
public:
    explicit LayoutProperty(T v) : value(std::move(v)) {}

    void parse(const char * name, const JSValue& layout) {
        if (layout.HasMember(name)) {
            parsedValue = parseProperty<Function<T>>(name, layout[name]);
        }
    }

    void calculate(const StyleCalculationParameters& parameters) {
        if (parsedValue) {
            value = (*parsedValue).evaluate(parameters);
        }
    }

    void operator=(const T& v) { value = v; }
    operator T() const { return value; }

    optional<Function<T>> parsedValue;
    T value;
};

} // namespace mbgl

#endif
