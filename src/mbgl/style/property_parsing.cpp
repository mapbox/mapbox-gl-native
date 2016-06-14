#include <mbgl/style/property_parsing.hpp>

#include <mbgl/platform/log.hpp>

#include <csscolorparser/csscolorparser.hpp>

namespace mbgl {
namespace style {

template <>
optional<bool> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsBool()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a boolean", name);
        return {};
    }

    return value.GetBool();
}

template <>
optional<float> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsNumber()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a number, or a number function", name);
        return {};
    }

    return value.GetDouble();
}

template <>
optional<std::string> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return std::string { value.GetString(), value.GetStringLength() };
}

template <>
optional<Color> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    CSSColorParser::Color css_color = CSSColorParser::parse({ value.GetString(), value.GetStringLength() });

    // Premultiply the color.
    const float factor = css_color.a / 255;

    return Color( (float)css_color.r * factor,
                  (float)css_color.g * factor,
                  (float)css_color.b * factor,
                  css_color.a );
}

template <>
optional<std::array<float, 2>> parseConstant(const char* name, const JSValue& value) {
    if (value.IsArray() && value.Size() == 2 &&
            value[rapidjson::SizeType(0)].IsNumber() &&
            value[rapidjson::SizeType(1)].IsNumber()) {

        float first = value[rapidjson::SizeType(0)].GetDouble();
        float second = value[rapidjson::SizeType(1)].GetDouble();
        return { {{ first, second }} };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be an array of two numbers", name);
        return {};
    }
}

template <>
optional<std::vector<float>> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be an array of numbers", name);
        return {};
    }

    std::vector<float> result;

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        const JSValue& part = value[i];

        if (!part.IsNumber()) {
            Log::Warning(Event::ParseStyle, "value of '%s' must be an array of numbers", name);
            return {};
        }

        result.push_back(part.GetDouble());
    }

    return result;
}

template <>
optional<std::vector<std::string>> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be an array of strings", name);
        return {};
    }

    std::vector<std::string> result;

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        const JSValue& part = value[i];

        if (!part.IsString()) {
            Log::Warning(Event::ParseStyle, "value of '%s' must be an array of strings", name);
            return {};
        }

        result.push_back({ part.GetString(), part.GetStringLength() });
    }

    return result;
}

optional<TransitionOptions> parseTransitionOptions(const char *, const JSValue& value) {
    if (!value.IsObject()) {
        return {};
    }

    optional<Duration> duration;
    if (value.HasMember("duration") && value["duration"].IsNumber()) {
        duration.emplace(Milliseconds(value["duration"].GetUint()));
    }

    optional<Duration> delay;
    if (value.HasMember("delay") && value["delay"].IsNumber()) {
        delay.emplace(Milliseconds(value["delay"].GetUint()));
    }

    if (!duration && !delay) {
        return {};
    }

    return TransitionOptions(duration, delay);
}

} // namespace style
} // namespace mbgl
