#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion/property_value.hpp>

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

template <typename T>
PropertyValue<T> parseProperty(const char* name, const JSValue& value) {
    conversion::Result<PropertyValue<T>> result = conversion::convert<PropertyValue<T>>(value);
    if (!result) {
        Log::Warning(Event::ParseStyle, "%s: %s", name, result.error().message);
        return {};
    }
    return *result;
}

optional<TransitionOptions> parseTransitionOptions(const char * name, const JSValue&);

} // namespace style
} // namespace mbgl
