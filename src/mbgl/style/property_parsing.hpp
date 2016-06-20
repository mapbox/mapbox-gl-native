#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion.hpp>

#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace style {

template <typename T>
PropertyValue<T> parseProperty(const char* name, const JSValue& value) {
    conversion::Result<PropertyValue<T>> result = conversion::convertPropertyValue<T>(value);
    if (result.template is<conversion::Error>()) {
        Log::Warning(Event::ParseStyle, "%s: %s", name, result.template get<conversion::Error>().message);
        return {};
    }
    return result.template get<PropertyValue<T>>();
}

optional<TransitionOptions> parseTransitionOptions(const char * name, const JSValue&);

} // namespace style
} // namespace mbgl
