#ifndef MBGL_LAYOUT_PROPERTY
#define MBGL_LAYOUT_PROPERTY

#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/function.hpp>

#include <rapidjson/document.h>

namespace mbgl {

using JSVal = rapidjson::Value;

template <typename T>
class LayoutProperty {
public:
    LayoutProperty(T v) : value(v) {}

    void parse(const char * name, const JSVal& layout) {
        if (layout.HasMember(name)) {
            parsedValue = parseProperty<Function<T>>(name, layout[name]);
        }
    }

    void calculate(const StyleCalculationParameters& parameters) {
        if (parsedValue) {
            value = (*parsedValue).evaluate(parameters);
        }
    }

    operator T() const { return value; }

    mapbox::util::optional<Function<T>> parsedValue;
    T value;
};

}

#endif
