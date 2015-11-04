#ifndef MBGL_PROPERTY_PARSING
#define MBGL_PROPERTY_PARSING

#include <mbgl/style/class_properties.hpp>

#include <mapbox/optional.hpp>
#include <rapidjson/document.h>

namespace mbgl {

using JSVal = rapidjson::Value;

template <typename T>
using optional = mapbox::util::optional<T>;

namespace detail {

template <typename T>
optional<T> parseProperty(const char* name, const JSVal&);

}

template <typename T>
void parseProperty(const char* name, PropertyKey key, ClassProperties& properties, const JSVal& value) {
    if (!value.HasMember(name))
        return;

    const optional<T> res = detail::parseProperty<T>(name, value[name]);

    if (res) {
        properties.set(key, *res);
    }
}

}

#endif
