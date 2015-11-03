#include <mbgl/style/property_parsing.hpp>

#include <mbgl/platform/log.hpp>

#include <csscolorparser/csscolorparser.hpp>

namespace mbgl {
namespace detail {

template <>
optional<bool> parseProperty(const char* name, const JSVal& value) {
    if (!value.IsBool()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a boolean", name);
        return {};
    }

    return value.GetBool();
}

template <>
optional<float> parseProperty(const char* name, const JSVal& value) {
    if (!value.IsNumber()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a number, or a number function", name);
        return {};
    }

    return value.GetDouble();
}

template <>
optional<std::string> parseProperty(const char* name, const JSVal& value) {
    if (std::string { "text-font" } == name) {
        if (!value.IsArray()) {
            Log::Warning(Event::ParseStyle, "value of '%s' must be an array of strings", name);
            return {};
        }

        std::string result = "";
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            const JSVal& stop = value[i];
            if (stop.IsString()) {
                result += stop.GetString();
                if (i < value.Size()-1) {
                    result += ",";
                }
            } else {
                Log::Warning(Event::ParseStyle, "text-font members must be strings");
                return {};
            }
        }
        return result;
    }

    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return std::string { value.GetString(), value.GetStringLength() };
}

template <>
optional<Color> parseProperty(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    CSSColorParser::Color css_color = CSSColorParser::parse({ value.GetString(), value.GetStringLength() });

    // Premultiply the color.
    const float factor = css_color.a / 255;

    return Color{{(float)css_color.r * factor,
                  (float)css_color.g * factor,
                  (float)css_color.b * factor,
                  css_color.a}};
}

template <>
optional<TranslateAnchorType> parseProperty(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TranslateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<RotateAnchorType> parseProperty<RotateAnchorType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { RotateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<CapType> parseProperty<CapType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { CapTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<JoinType> parseProperty<JoinType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { JoinTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<PlacementType> parseProperty<PlacementType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { PlacementTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<TextAnchorType> parseProperty<TextAnchorType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TextAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<TextJustifyType> parseProperty<TextJustifyType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TextJustifyTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<TextTransformType> parseProperty<TextTransformType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TextTransformTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<RotationAlignmentType> parseProperty<RotationAlignmentType>(const char* name, const JSVal& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { RotationAlignmentTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template <>
optional<std::array<float, 2>> parseProperty(const char* name, const JSVal& value) {
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
optional<std::vector<float>> parseProperty(const char* name, const JSVal& value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be an array of numbers", name);
        return {};
    }

    std::vector<float> result;

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        const JSVal& part = value[i];

        if (!part.IsNumber()) {
            Log::Warning(Event::ParseStyle, "value of '%s' must be an array of numbers", name);
            return {};
        }

        result.push_back(part.GetDouble());
    }

    return result;
}

template <>
optional<PropertyTransition> parseProperty(const char *, const JSVal& value) {
    PropertyTransition transition;
    if (value.IsObject()) {
        bool parsed = false;
        if (value.HasMember("duration") && value["duration"].IsNumber()) {
            transition.duration = std::chrono::milliseconds(value["duration"].GetUint());
            parsed = true;
        }
        if (value.HasMember("delay") && value["delay"].IsNumber()) {
            transition.delay = std::chrono::milliseconds(value["delay"].GetUint());
            parsed = true;
        }
        if (!parsed) {
            return {};
        }
    }
    return transition;
}

// --- Function ---

template <typename T>
optional<std::vector<std::pair<float, T>>> parseStops(const char* name, const JSVal& value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "stops function must specify a stops array");
        return {};
    }

    std::vector<std::pair<float, T>> stops;

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        const JSVal& stop = value[i];

        if (!stop.IsArray()) {
            Log::Warning(Event::ParseStyle, "function argument must be a numeric value");
            return {};
        }

        if (stop.Size() != 2) {
            Log::Warning(Event::ParseStyle, "stop must have zoom level and value specification");
            return {};
        }

        const JSVal& z = stop[rapidjson::SizeType(0)];
        if (!z.IsNumber()) {
            Log::Warning(Event::ParseStyle, "zoom level in stop must be a number");
            return {};
        }

        optional<T> v = parseProperty<T>(name, stop[rapidjson::SizeType(1)]);
        if (!v) {
            return {};
        }

        stops.emplace_back(z.GetDouble(), *v);
    }

    return stops;
}

template <typename T>
optional<Function<T>> parseFunction(const char* name, const JSVal& value) {
    if (!value.IsObject()) {
        auto constant = parseProperty<T>(name, value);
        if (!constant) {
            return {};
        }
        return { Function<T>(*constant) };
    }

    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return {};
    }

    float base = 1.0f;

    if (value.HasMember("base")) {
        const JSVal& value_base = value["base"];

        if (!value_base.IsNumber()) {
            Log::Warning(Event::ParseStyle, "base must be numeric");
            return {};
        }

        base = value_base.GetDouble();
    }

    auto stops = parseStops<T>(name, value["stops"]);

    if (!stops) {
        return {};
    }

    return { Function<T>(*stops, base) };
}

template <> optional<Function<std::array<float, 2>>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<std::array<float, 2>>(name, value);
}

template <> optional<Function<std::string>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<std::string>(name, value);
}

template <> optional<Function<TranslateAnchorType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<TranslateAnchorType>(name, value);
}

template <> optional<Function<RotateAnchorType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<RotateAnchorType>(name, value);
}

template <> optional<Function<CapType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<CapType>(name, value);
}

template <> optional<Function<JoinType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<JoinType>(name, value);
}

template <> optional<Function<PlacementType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<PlacementType>(name, value);
}

template <> optional<Function<TextAnchorType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<TextAnchorType>(name, value);
}

template <> optional<Function<TextJustifyType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<TextJustifyType>(name, value);
}

template <> optional<Function<TextTransformType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<TextTransformType>(name, value);
}

template <> optional<Function<RotationAlignmentType>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<RotationAlignmentType>(name, value);
}

template <> optional<Function<bool>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<bool>(name, value);
}

template<> optional<Function<float>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<float>(name, value);
}

template<> optional<Function<Color>> parseProperty(const char* name, const JSVal& value) {
    return parseFunction<Color>(name, value);
}

template <typename T>
optional<Function<Faded<T>>> parseFadedFunction(const JSVal& value, const JSVal& transition) {
    mapbox::util::optional<Duration> duration;
    if (transition.HasMember("duration")) {
        duration = std::chrono::milliseconds(transition["duration"].GetUint());
    }

    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return {};
    }

    auto stops = parseStops<T>("", value["stops"]);

    if (!stops) {
        return {};
    }

    return Function<Faded<T>>(*stops, duration);
}

template <>
optional<Function<Faded<std::vector<float>>>> parseProperty(const char* name, const JSVal& value, const JSVal& transition) {
    if (value.IsObject()) {
        return parseFadedFunction<std::vector<float>>(value, transition);
    }

    auto constant = parseProperty<std::vector<float>>(name, value);
    if (!constant) {
        return {};
    }
    return Function<Faded<std::vector<float>>>(*constant);
}

template <>
optional<Function<Faded<std::string>>> parseProperty(const char* name, const JSVal& value, const JSVal& transition) {
    if (value.IsObject()) {
        return parseFadedFunction<std::string>(value, transition);
    }

    auto constant = parseProperty<std::string>(name, value);
    if (!constant) {
        return {};
    }
    return Function<Faded<std::string>>(*constant);
}

}
}
