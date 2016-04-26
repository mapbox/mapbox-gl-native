#include <mbgl/style/property_parsing.hpp>

#include <csscolorparser/csscolorparser.hpp>

namespace mbgl {

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

    return Color{{(float)css_color.r * factor,
                  (float)css_color.g * factor,
                  (float)css_color.b * factor,
                  css_color.a}};
}

MBGL_DEFINE_ENUM_CLASS(TranslateAnchorTypeClass, TranslateAnchorType, {
    { TranslateAnchorType::Map, "map" },
    { TranslateAnchorType::Viewport, "viewport" },
});

template <>
optional<TranslateAnchorType> parseConstant(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TranslateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(RotateAnchorTypeClass, RotateAnchorType, {
    { RotateAnchorType::Map, "map" },
    { RotateAnchorType::Viewport, "viewport" },
});

template <>
optional<RotateAnchorType> parseConstant<RotateAnchorType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { RotateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(LineCapTypeClass, LineCapType, {
    { LineCapType::Round, "round" },
    { LineCapType::Butt, "butt" },
    { LineCapType::Square, "square" },
});

template <>
optional<LineCapType> parseConstant<LineCapType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { LineCapTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(LineJoinTypeClass, LineJoinType, {
    { LineJoinType::Miter, "miter" },
    { LineJoinType::Bevel, "bevel" },
    { LineJoinType::Round, "round" },
    { LineJoinType::FakeRound, "fakeround" },
    { LineJoinType::FlipBevel, "flipbevel" },
});

template <>
optional<LineJoinType> parseConstant<LineJoinType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { LineJoinTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(SymbolPlacementTypeClass, SymbolPlacementType, {
    { SymbolPlacementType::Point, "point" },
    { SymbolPlacementType::Line, "line" },
});

template <>
optional<SymbolPlacementType> parseConstant<SymbolPlacementType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { SymbolPlacementTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(TextAnchorTypeClass, TextAnchorType, {
    { TextAnchorType::Center, "center" },
    { TextAnchorType::Left, "left" },
    { TextAnchorType::Right, "right" },
    { TextAnchorType::Top, "top" },
    { TextAnchorType::Bottom, "bottom" },
    { TextAnchorType::TopLeft, "top-left" },
    { TextAnchorType::TopRight, "top-right" },
    { TextAnchorType::BottomLeft, "bottom-left" },
    { TextAnchorType::BottomRight, "bottom-right" }
});

template <>
optional<TextAnchorType> parseConstant<TextAnchorType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TextAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(TextJustifyTypeClass, TextJustifyType, {
    { TextJustifyType::Center, "center" },
    { TextJustifyType::Left, "left" },
    { TextJustifyType::Right, "right" },
});

template <>
optional<TextJustifyType> parseConstant<TextJustifyType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TextJustifyTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(TextTransformTypeClass, TextTransformType, {
    { TextTransformType::None, "none" },
    { TextTransformType::Uppercase, "uppercase" },
    { TextTransformType::Lowercase, "lowercase" },
});

template <>
optional<TextTransformType> parseConstant<TextTransformType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { TextTransformTypeClass({ value.GetString(), value.GetStringLength() }) };
}

MBGL_DEFINE_ENUM_CLASS(RotationAlignmentTypeClass, RotationAlignmentType, {
    { RotationAlignmentType::Map, "map" },
    { RotationAlignmentType::Viewport, "viewport" },
});

template <>
optional<RotationAlignmentType> parseConstant<RotationAlignmentType>(const char* name, const JSValue& value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", name);
        return {};
    }

    return { RotationAlignmentTypeClass({ value.GetString(), value.GetStringLength() }) };
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

optional<PropertyTransition> parsePropertyTransition(const char *, const JSValue& value) {
    PropertyTransition transition;
    if (value.IsObject()) {
        bool parsed = false;
        if (value.HasMember("duration") && value["duration"].IsNumber()) {
            transition.duration.emplace(Milliseconds(value["duration"].GetUint()));
            parsed = true;
        }
        if (value.HasMember("delay") && value["delay"].IsNumber()) {
            transition.delay.emplace(Milliseconds(value["delay"].GetUint()));
            parsed = true;
        }
        if (!parsed) {
            return {};
        }
    }
    return transition;
}

} // namespace mbgl
