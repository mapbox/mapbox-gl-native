#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/color.hpp>

#include <mbgl/platform/log.hpp>

#include <string>
#include <array>
#include <vector>

namespace mbgl {
namespace style {

template <typename T>
optional<T> parseConstant(const char* name, const JSValue&);

template <> optional<bool> parseConstant(const char*, const JSValue&);
template <> optional<float> parseConstant(const char*, const JSValue&);
template <> optional<std::string> parseConstant(const char*, const JSValue&);
template <> optional<Color> parseConstant(const char*, const JSValue&);
template <> optional<TranslateAnchorType> parseConstant(const char*, const JSValue&);
template <> optional<RotateAnchorType> parseConstant(const char*, const JSValue&);
template <> optional<LineCapType> parseConstant(const char*, const JSValue&);
template <> optional<LineJoinType> parseConstant(const char*, const JSValue&);
template <> optional<SymbolPlacementType> parseConstant(const char*, const JSValue&);
template <> optional<TextAnchorType> parseConstant(const char*, const JSValue&);
template <> optional<TextJustifyType> parseConstant(const char*, const JSValue&);
template <> optional<TextTransformType> parseConstant(const char*, const JSValue&);
template <> optional<AlignmentType> parseConstant(const char*, const JSValue&);
template <> optional<std::array<float, 2>> parseConstant(const char*, const JSValue&);
template <> optional<std::vector<float>> parseConstant(const char*, const JSValue&);
template <> optional<std::vector<std::string>> parseConstant(const char*, const JSValue&);

template <typename T>
PropertyValue<T> parseProperty(const char* name, const JSValue& value) {
    if (!value.IsObject()) {
        auto constant = parseConstant<T>(name, value);

        if (!constant) {
            return {};
        }

        return *constant;
    }

    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return {};
    }

    float base = 1.0f;

    if (value.HasMember("base")) {
        const JSValue& value_base = value["base"];

        if (!value_base.IsNumber()) {
            Log::Warning(Event::ParseStyle, "base must be numeric");
            return {};
        }

        base = value_base.GetDouble();
    }

    const JSValue& stopsValue = value["stops"];

    if (!stopsValue.IsArray()) {
        Log::Warning(Event::ParseStyle, "stops function must specify a stops array");
        return {};
    }

    std::vector<std::pair<float, T>> stops;

    for (rapidjson::SizeType i = 0; i < stopsValue.Size(); ++i) {
        const JSValue& stop = stopsValue[i];

        if (!stop.IsArray()) {
            Log::Warning(Event::ParseStyle, "function argument must be a numeric value");
            return {};
        }

        if (stop.Size() != 2) {
            Log::Warning(Event::ParseStyle, "stop must have zoom level and value specification");
            return {};
        }

        const JSValue& z = stop[rapidjson::SizeType(0)];
        if (!z.IsNumber()) {
            Log::Warning(Event::ParseStyle, "zoom level in stop must be a number");
            return {};
        }

        optional<T> v = parseConstant<T>(name, stop[rapidjson::SizeType(1)]);
        if (!v) {
            return {};
        }

        stops.emplace_back(z.GetDouble(), *v);
    }

    return Function<T>(stops, base);
}

optional<TransitionOptions> parseTransitionOptions(const char * name, const JSValue&);

} // namespace style
} // namespace mbgl
