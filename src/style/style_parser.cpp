#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/platform/log.hpp>
#include <csscolorparser/csscolorparser.hpp>

namespace mbgl {

using JSVal = const rapidjson::Value&;

StyleParser::StyleParser() {
}

void StyleParser::parse(JSVal document) {
    if (document.HasMember("constants")) {
        parseConstants(document["constants"]);
    }

    if (document.HasMember("sources")) {
        parseSources(document["sources"]);
    }

    if (document.HasMember("layers")) {
        root = createLayers(document["layers"]);
        parseLayers();
    }

    if (document.HasMember("sprite")) {
        parseSprite(document["sprite"]);
    }

    if (document.HasMember("glyphs")) {
        parseGlyphURL(document["glyphs"]);
    }
}

void StyleParser::parseConstants(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            // Discard constants that don't start with an @ sign.
            if (name.length() && name[0] == '@') {
                constants.emplace(std::move(name), &itr->value);
            }
        }
    } else {
        Log::Warning(Event::ParseStyle, "constants must be an object");
    }
}

JSVal StyleParser::replaceConstant(JSVal value) {
    if (value.IsString()) {
        auto it = constants.find({ value.GetString(), value.GetStringLength() });
        if (it != constants.end()) {
            return *it->second;
        }
    }

    return value;
}

#pragma mark - Parse Render Properties

template<> bool StyleParser::parseRenderProperty(JSVal value, bool &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsBool()) {
            target = property.GetBool();
            return true;
        } else {
            fprintf(stderr, "[WARNING] '%s' must be a boolean\n", name);
        }
    }
    return false;
}


template<> bool StyleParser::parseRenderProperty(JSVal value, std::string &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsString()) {
            target = { property.GetString(), property.GetStringLength() };
            return true;
        } else {
            Log::Warning(Event::ParseStyle, "'%s' must be a string", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, float &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsNumber()) {
            target = property.GetDouble();
            return true;
        } else {
            Log::Warning(Event::ParseStyle, "'%s' must be a number", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, uint16_t &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsUint()) {
            unsigned int value = property.GetUint();
            if (value > std::numeric_limits<uint16_t>::max()) {
                Log::Warning(Event::ParseStyle, "values for %s that are larger than %d are not supported", name, std::numeric_limits<uint16_t>::max());
                return false;
            }

            target = value;
            return true;
        } else {
            Log::Warning(Event::ParseStyle, "%s must be an unsigned integer", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, int32_t &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsInt()) {
            target = property.GetInt();
            return true;
        } else {
            Log::Warning(Event::ParseStyle, "%s must be an integer", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, vec2<float> &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsArray()) {
            if (property.Size() >= 2) {
                target.x = property[(rapidjson::SizeType)0].GetDouble();
                target.y = property[(rapidjson::SizeType)1].GetDouble();
                return true;
            } else {
                Log::Warning(Event::ParseStyle, "%s must have at least two members", name);
            }
        } else {
            Log::Warning(Event::ParseStyle, "%s must be an array of numbers", name);
        }
    }
    return false;
}

template<typename Parser, typename T>
bool StyleParser::parseRenderProperty(JSVal value, T &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsString()) {
            target = Parser({ property.GetString(), property.GetStringLength() });
            return true;
        } else {
            Log::Warning(Event::ParseStyle, "%s must have one of the enum values", name);
        }
    }
    return false;
}


#pragma mark - Parse Sources

void StyleParser::parseSources(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            SourceType type = SourceType::Vector;
            std::string url;
            uint16_t tile_size = 512;
            int32_t min_zoom = 0;
            int32_t max_zoom = 22;

            parseRenderProperty<SourceTypeClass>(itr->value, type, "type");
            parseRenderProperty(itr->value, url, "url");
            if (type == SourceType::Raster) {
                parseRenderProperty(itr->value, tile_size, "tileSize");
            }
            parseRenderProperty(itr->value, min_zoom, "minZoom");
            parseRenderProperty(itr->value, max_zoom, "maxZoom");

            sources.emplace(std::move(name), std::make_shared<StyleSource>(SourceInfo { type, url, tile_size, min_zoom, max_zoom }));
        }
    } else {
        Log::Warning(Event::ParseStyle, "sources must be an object");
    }
}

#pragma mark - Parse Style Properties

Color parseColor(JSVal value) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "color value must be a string");
        return Color{{ 0, 0, 0, 0 }};
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
bool StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsBool()) {
        return rvalue.GetBool();
    } else if (rvalue.IsNumber()) {
        return rvalue.GetDouble();
    } else {
        Log::Warning(Event::ParseStyle, "function argument must be a boolean or numeric value");
        return false;
    }
}

template <>
float StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsNumber()) {
        return rvalue.GetDouble();
    } else {
        Log::Warning(Event::ParseStyle, "function argument must be a numeric value");
        return 0.0f;
    }
}

template <>
Color StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    return parseColor(rvalue);
}

template <typename T> inline float defaultBaseValue() { return 1.75; }
template <> inline float defaultBaseValue<Color>() { return 1.0; }

template <typename T>
std::tuple<bool, Function<T>> StyleParser::parseFunction(JSVal value) {
    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return std::tuple<bool, Function<T>> { false, ConstantFunction<T>(T()) };
    }

    float base = defaultBaseValue<T>();

    if (value.HasMember("base")) {
        JSVal value_base = value["base"];
        if (value_base.IsNumber()) {
            base = value_base.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "base must be numeric");
        }
    }

    JSVal value_stops = value["stops"];
    if (!value_stops.IsArray()) {
        Log::Warning(Event::ParseStyle, "stops function must specify a stops array");
        return std::tuple<bool, Function<T>> { false, ConstantFunction<T>(T()) };
    }

    std::vector<std::pair<float, T>> stops;
    for (rapidjson::SizeType i = 0; i < value_stops.Size(); ++i) {
        JSVal stop = value_stops[i];
        if (stop.IsArray()) {
            if (stop.Size() != 2) {
                Log::Warning(Event::ParseStyle, "stop must have zoom level and value specification");
                return std::tuple<bool, Function<T>> { false, ConstantFunction<T>(T()) };
            }

            JSVal z = stop[rapidjson::SizeType(0)];
            if (!z.IsNumber()) {
                Log::Warning(Event::ParseStyle, "zoom level in stop must be a number");
                return std::tuple<bool, Function<T>> { false, ConstantFunction<T>(T()) };
            }

            stops.emplace_back(z.GetDouble(), parseFunctionArgument<T>(stop[rapidjson::SizeType(1)]));
        } else {
            Log::Warning(Event::ParseStyle, "function argument must be a numeric value");
            return std::tuple<bool, Function<T>> { false, ConstantFunction<T>(T()) };
        }
    }

    return std::tuple<bool, Function<T>> { true, StopsFunction<T>(stops, base) };
}


template <typename T>
bool StyleParser::parseFunction(PropertyKey key, ClassProperties &klass, JSVal value) {
    bool parsed;
    Function<T> function;
    std::tie(parsed, function) = parseFunction<T>(value);
    if (parsed) {
        klass.set(key, function);
    }
    return parsed;
}

template <typename T>
bool StyleParser::setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass) {
    bool parsed;
    T result;
    std::tie(parsed, result) = parseProperty<T>(value, property_name);
    if (parsed) {
        klass.set(key, result);
    }
    return parsed;
}

template <typename T>
bool StyleParser::setProperty(JSVal value, const char *property_name, T &target) {
    bool parsed;
    T result;
    std::tie(parsed, result) = parseProperty<T>(value, property_name);
    if (parsed) {
        target = std::move(result);
    }
    return parsed;
}


template<typename T>
bool StyleParser::parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    } else {
        return setProperty<T>(replaceConstant(value[property_name]), property_name, key, klass);
    }
}

template <typename T>
bool StyleParser::parseOptionalProperty(const char *property_name, T &target, JSVal value) {
    if (!value.HasMember(property_name)) {
         return false;
    } else {
        return setProperty<T>(replaceConstant(value[property_name]), property_name, target);
    }
}

template<> std::tuple<bool, std::string> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, std::string> { false, std::string() };
    }

    return std::tuple<bool, std::string> { true, { value.GetString(), value.GetStringLength() } };
}

template<> std::tuple<bool, TranslateAnchorType> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, TranslateAnchorType> { false, TranslateAnchorType::Map };
    }

    return std::tuple<bool, TranslateAnchorType> { true, TranslateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, RotateAnchorType> StyleParser::parseProperty<RotateAnchorType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, RotateAnchorType> { false, RotateAnchorType::Map };
    }

    return std::tuple<bool, RotateAnchorType> { true, RotateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, PropertyTransition> StyleParser::parseProperty(JSVal value, const char */*property_name*/) {
    PropertyTransition transition;
    if (value.IsObject()) {
        if (value.HasMember("duration") && value["duration"].IsNumber()) {
            transition.duration = value["duration"].GetUint();
        }
        if (value.HasMember("delay") && value["delay"].IsNumber()) {
            transition.delay = value["delay"].GetUint();
        }
    }

    if (transition.duration == 0 && transition.delay == 0) {
        return std::tuple<bool, PropertyTransition> { false, std::move(transition) };
    }

    return std::tuple<bool, PropertyTransition> { true, std::move(transition) };
}

template<> std::tuple<bool, Function<bool>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (value.IsObject()) {
        return parseFunction<bool>(value);
    } else if (value.IsNumber()) {
        return std::tuple<bool, Function<bool>> { true, ConstantFunction<bool>(value.GetDouble()) };
    } else if (value.IsBool()) {
        return std::tuple<bool, Function<bool>> { true, ConstantFunction<bool>(value.GetBool()) };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be convertible to boolean, or a boolean function", property_name);
        return std::tuple<bool, Function<bool>> { false, ConstantFunction<bool>(false) };
    }
}

template<> std::tuple<bool, Function<float>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (value.IsObject()) {
        return parseFunction<float>(value);
    } else if (value.IsNumber()) {
        return std::tuple<bool, Function<float>> { true, ConstantFunction<float>(value.GetDouble()) };
    } else if (value.IsBool()) {
        return std::tuple<bool, Function<float>> { true, ConstantFunction<float>(value.GetBool()) };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a number, or a number function", property_name);
        return std::tuple<bool, Function<float>> { false, ConstantFunction<float>(0) };
    }
}

template<> std::tuple<bool, Function<Color>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (value.IsObject()) {
        return parseFunction<Color>(value);
    } else if (value.IsString()) {
        return std::tuple<bool, Function<Color>> { true, ConstantFunction<Color>(parseColor(value)) };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a color, or a color function", property_name);
        return std::tuple<bool, Function<Color>> { false, ConstantFunction<Color>(Color {{ 0, 0, 0, 0 }}) };
    }
}

template <typename T>
bool StyleParser::parseOptionalProperty(const char *property_name, const std::vector<PropertyKey> &keys, ClassProperties &klass, JSVal value) {
    if (value.HasMember(property_name)) {
        JSVal rvalue = replaceConstant(value[property_name]);
        if (!rvalue.IsArray()) {
            Log::Warning(Event::ParseStyle, "array value must be an array");
        }

        if (rvalue.Size() != keys.size()) {
            Log::Warning(Event::ParseStyle, "array value has unexpected number of elements");
        }

        for (uint16_t i = 0; i < keys.size(); i++) {
            setProperty<T>(rvalue[(rapidjson::SizeType)i], property_name, keys[i], klass);
        }
    }
    return true;
}

#pragma mark - Parse Layers

std::unique_ptr<StyleLayerGroup> StyleParser::createLayers(JSVal value) {
    if (value.IsArray()) {
        std::unique_ptr<StyleLayerGroup> group = std::make_unique<StyleLayerGroup>();
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            std::shared_ptr<StyleLayer> layer = createLayer(value[i]);
            if (layer) {
                group->layers.emplace_back(layer);
            }
        }
        return group;
    } else {
        Log::Warning(Event::ParseStyle, "layers must be an array");
        return nullptr;
    }
}

std::shared_ptr<StyleLayer> StyleParser::createLayer(JSVal value) {
    if (value.IsObject()) {
        if (!value.HasMember("id")) {
            Log::Warning(Event::ParseStyle, "layer must have an id");
            return nullptr;
        }

        JSVal id = value["id"];
        if (!id.IsString()) {
            Log::Warning(Event::ParseStyle, "layer id must be a string");
            return nullptr;
        }

        const std::string layer_id = { id.GetString(), id.GetStringLength() };

        if (layers.find(layer_id) != layers.end()) {
            Log::Warning(Event::ParseStyle, "duplicate layer id %s", layer_id.c_str());
            return nullptr;
        }

        // Parse styles already, as they can't be inherited anyway.
        std::map<ClassID, ClassProperties> styles;
        parseStyles(value, styles);

        std::shared_ptr<StyleLayer> layer = std::make_shared<StyleLayer>(
            layer_id, std::move(styles));

        if (value.HasMember("layers")) {
            layer->layers = createLayers(value["layers"]);
        }

        // Store the layer ID so we can reference it later.
        layers.emplace(layer_id, std::pair<JSVal, std::shared_ptr<StyleLayer>> { value, layer });

        return layer;
    } else {
        Log::Warning(Event::ParseStyle, "layer must be an object");
        return nullptr;
    }
}

void StyleParser::parseLayers() {
    for (std::pair<const std::string, std::pair<JSVal, std::shared_ptr<StyleLayer>>> &pair : layers) {
        parseLayer(pair.second);
    }
}

void StyleParser::parseLayer(std::pair<JSVal, std::shared_ptr<StyleLayer>> &pair) {
    JSVal value = pair.first;
    std::shared_ptr<StyleLayer> &layer = pair.second;

    if (value.HasMember("type")) {
        JSVal type = value["type"];
        if (!type.IsString()) {
            Log::Warning(Event::ParseStyle, "layer type of '%s' must be a string", layer->id.c_str());
        } else {
            layer->type = StyleLayerTypeClass(std::string { type.GetString(), type.GetStringLength() });
        }
    }

    if (layer->bucket || (layer->layers && layer->type != StyleLayerType::Raster)) {
        // Skip parsing this again. We already have a valid layer definition.
        return;
    }

    // Make sure we have not previously attempted to parse this layer.
    if (std::find(stack.begin(), stack.end(), layer.get()) != stack.end()) {
        Log::Warning(Event::ParseStyle, "layer reference of '%s' is circular", layer->id.c_str());
        return;
    }

    if (value.HasMember("ref")) {
        // This layer is referencing another layer. Inherit the bucket from that layer, if we
        // already parsed it.
        parseReference(replaceConstant(value["ref"]), layer);
    } else {
        // Otherwise, parse the source/source-layer/filter/render keys to form the bucket.
        parseBucket(value, layer);
    }
}

#pragma mark - Parse Styles

void StyleParser::parseStyles(JSVal value, std::map<ClassID, ClassProperties> &styles) {
    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const std::string name { itr->name.GetString(), itr->name.GetStringLength() };

        if (name == "style") {
            parseStyle(replaceConstant(itr->value), styles[ClassID::Default]);
        } else if (name.compare(0, 6, "style.") == 0 && name.length() > 6) {
            const ClassID class_id = ClassDictionary::Lookup(name.substr(6));
            parseStyle(replaceConstant(itr->value), styles[class_id]);
        }
    }
}

void StyleParser::parseStyle(JSVal value, ClassProperties &klass) {
    using Key = PropertyKey;

    parseOptionalProperty<Function<bool>>("fill-antialias", Key::FillAntialias, klass, value);
    parseOptionalProperty<Function<float>>("fill-opacity", Key::FillOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-fill-opacity", Key::FillOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("fill-color", Key::FillColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-fill-color", Key::FillColor, klass, value);
    parseOptionalProperty<Function<Color>>("fill-outline-color", Key::FillOutlineColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-fill-outline-color", Key::FillOutlineColor, klass, value);
    parseOptionalProperty<Function<float>>("fill-translate", { Key::FillTranslateX, Key::FillTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-fill-translate", Key::FillTranslate, klass, value);
    parseOptionalProperty<TranslateAnchorType>("fill-translate-anchor", Key::FillTranslateAnchor, klass, value);
    parseOptionalProperty<std::string>("fill-image", Key::FillImage, klass, value);

    parseOptionalProperty<Function<float>>("line-opacity", Key::LineOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-opacity", Key::LineOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("line-color", Key::LineColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-color", Key::LineColor, klass, value);
    parseOptionalProperty<Function<float>>("line-translate", { Key::LineTranslateX, Key::LineTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-translate", Key::LineTranslate, klass, value);
    parseOptionalProperty<TranslateAnchorType>("line-translate-anchor", Key::LineTranslateAnchor, klass, value);
    parseOptionalProperty<Function<float>>("line-width", Key::LineWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-width", Key::LineWidth, klass, value);
    parseOptionalProperty<Function<float>>("line-offset", Key::LineOffset, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-offset", Key::LineOffset, klass, value);
    parseOptionalProperty<Function<float>>("line-blur", Key::LineBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-blur", Key::LineBlur, klass, value);
    parseOptionalProperty<Function<float>>("line-dasharray", { Key::LineDashLand, Key::LineDashGap }, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-line-dasharray", Key::LineDashArray, klass, value);
    parseOptionalProperty<std::string>("line-image", Key::LineImage, klass, value);

    parseOptionalProperty<Function<float>>("icon-opacity", Key::IconOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-opacity", Key::IconOpacity, klass, value);
    parseOptionalProperty<Function<float>>("icon-rotate", Key::IconRotate, klass, value);
    parseOptionalProperty<Function<float>>("icon-size", Key::IconSize, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-size", Key::IconSize, klass, value);
    parseOptionalProperty<Function<Color>>("icon-color", Key::IconColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-color", Key::IconColor, klass, value);
    parseOptionalProperty<Function<Color>>("icon-halo-color", Key::IconHaloColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-halo-color", Key::IconHaloColor, klass, value);
    parseOptionalProperty<Function<float>>("icon-halo-width", Key::IconHaloWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-halo-width", Key::IconHaloWidth, klass, value);
    parseOptionalProperty<Function<float>>("icon-halo-blur", Key::IconHaloBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-halo-blur", Key::IconHaloBlur, klass, value);
    parseOptionalProperty<Function<float>>("icon-translate", { Key::IconTranslateX, Key::IconTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-icon-translate", Key::IconTranslate, klass, value);

    parseOptionalProperty<Function<float>>("text-opacity", Key::TextOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-opacity", Key::TextOpacity, klass, value);
    parseOptionalProperty<Function<float>>("text-size", Key::TextSize, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-size", Key::TextSize, klass, value);
    parseOptionalProperty<Function<Color>>("text-color", Key::TextColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-color", Key::TextColor, klass, value);
    parseOptionalProperty<Function<Color>>("text-halo-color", Key::TextHaloColor, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-halo-color", Key::TextHaloColor, klass, value);
    parseOptionalProperty<Function<float>>("text-halo-width", Key::TextHaloWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-halo-width", Key::TextHaloWidth, klass, value);
    parseOptionalProperty<Function<float>>("text-halo-blur", Key::TextHaloBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-halo-blur", Key::TextHaloBlur, klass, value);
    parseOptionalProperty<Function<float>>("text-translate", { Key::TextTranslateX, Key::TextTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-text-translate", Key::TextTranslate, klass, value);

    parseOptionalProperty<Function<float>>("raster-opacity", Key::RasterOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-raster-opacity", Key::RasterOpacity, klass, value);
    parseOptionalProperty<Function<float>>("raster-hue-rotate", Key::RasterHueRotate, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-raster-hue-rotate", Key::RasterHueRotate, klass, value);
    parseOptionalProperty<Function<float>>("raster-brightness", { Key::RasterBrightnessLow, Key::RasterBrightnessHigh }, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-raster-brightness", Key::RasterBrightness, klass, value);
    parseOptionalProperty<Function<float>>("raster-saturation", Key::RasterSaturation, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-raster-saturation", Key::RasterSaturation, klass, value);
    parseOptionalProperty<Function<float>>("raster-contrast", Key::RasterContrast, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-raster-contrast", Key::RasterContrast, klass, value);
    parseOptionalProperty<Function<float>>("raster-fade", Key::RasterFade, klass, value);
    parseOptionalProperty<PropertyTransition>("transition-raster-fade", Key::RasterFade, klass, value);

    parseOptionalProperty<Function<Color>>("background-color", Key::BackgroundColor, klass, value);
}

void StyleParser::parseReference(JSVal value, std::shared_ptr<StyleLayer> &layer) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "layer ref of '%s' must be a string", layer->id.c_str());
        return;
    }
    const std::string ref { value.GetString(), value.GetStringLength() };
    auto it = layers.find(ref);
    if (it == layers.end()) {
        Log::Warning(Event::ParseStyle, "layer '%s' references unknown layer %s", layer->id.c_str(), ref.c_str());
        // We cannot parse this layer further.
        return;
    }

    // Recursively parse the referenced layer.
    stack.push_front(layer.get());
    parseLayer(it->second);
    stack.pop_front();


    std::shared_ptr<StyleLayer> reference = it->second.second;

    layer->type = reference->type;

    if (reference->layers) {
        Log::Warning(Event::ParseStyle, "layer '%s' references composite layer", layer->id.c_str());
        // We cannot parse this layer further.
        return;
    } else {
        layer->bucket = reference->bucket;
    }
}

#pragma mark - Parse Bucket

void StyleParser::parseBucket(JSVal value, std::shared_ptr<StyleLayer> &layer) {
    layer->bucket = std::make_shared<StyleBucket>(layer->type);

    // We name the buckets according to the layer that defined it.
    layer->bucket->name = layer->id;

    if (value.HasMember("source")) {
        JSVal value_source = replaceConstant(value["source"]);
        if (value_source.IsString()) {
            const std::string source_name = { value_source.GetString(), value_source.GetStringLength() };
            auto source_it = sources.find(source_name);
            if (source_it != sources.end()) {
                layer->bucket->style_source = source_it->second;
            } else {
                Log::Warning(Event::ParseStyle, "can't find source '%s' required for layer '%s'", source_name.c_str(), layer->id.c_str());
            }
        } else {
            Log::Warning(Event::ParseStyle, "source of layer '%s' must be a string", layer->id.c_str());
        }
    }

    if (value.HasMember("source-layer")) {
        JSVal value_source_layer = replaceConstant(value["source-layer"]);
        if (value_source_layer.IsString()) {
            layer->bucket->source_layer = { value_source_layer.GetString(), value_source_layer.GetStringLength() };
        } else {
            Log::Warning(Event::ParseStyle, "source-layer of layer '%s' must be a string", layer->id.c_str());
        }
    }

    if (value.HasMember("filter")) {
        JSVal value_filter = replaceConstant(value["filter"]);
        layer->bucket->filter = parseFilter(value_filter);
    }

    if (value.HasMember("render")) {
        JSVal value_render = replaceConstant(value["render"]);
        parseRender(value_render, layer);
    }

    if (value.HasMember("min-zoom")) {
        JSVal min_zoom = value["min-zoom"];
        if (min_zoom.IsNumber()) {
            layer->bucket->min_zoom = min_zoom.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "min-zoom of layer %s must be numeric", layer->id.c_str());
        }
    }

    if (value.HasMember("max-zoom")) {
        JSVal max_zoom = value["max-zoom"];
        if (max_zoom.IsNumber()) {
            layer->bucket->min_zoom = max_zoom.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "max-zoom of layer %s must be numeric", layer->id.c_str());
        }
    }
}

FilterExpression StyleParser::parseFilter(JSVal value) {
    return parseFilter(value, value.IsArray() ? FilterExpression::Operator::Or : FilterExpression::Operator::And);
}

FilterExpression StyleParser::parseFilter(JSVal value, FilterExpression::Operator op) {
    FilterExpression expression(op);
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            const std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            if (name == "&") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::And));
            } else if (name == "|") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::Or));
            } else if (name == "^") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::Xor));
            } else if (name == "!") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::Nor));
            } else if (name == "$type") {
                JSVal type = replaceConstant(itr->value);
                if (type.IsString()) {
                    expression.setGeometryType(parseGeometryType({ type.GetString(), type.GetStringLength() }));
                }
            } else {
                FilterComparison comparison(name);
                JSVal filterValue = replaceConstant(itr->value);
                if (filterValue.IsObject()) {
                    rapidjson::Value::ConstMemberIterator itr = filterValue.MemberBegin();
                    for (; itr != filterValue.MemberEnd(); ++itr) {
                        comparison.add(
                            parseFilterComparisonOperator({ itr->name.GetString(), itr->name.GetStringLength() }),
                            parseValues(replaceConstant(itr->value))
                        );
                    }
                } else if (filterValue.IsArray()) {
                    comparison.add(FilterComparison::Operator::In, parseValues(filterValue));
                } else {
                    comparison.add(FilterComparison::Operator::Equal, std::vector<Value>({ parseValue(filterValue) }));
                }
                expression.add(comparison);
            }
        }
    } else if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); i++) {
            expression.add(parseFilter(replaceConstant(value[i])));
        }
    } else {
        Log::Warning(Event::ParseStyle, "expression must be either an array or an object");
    }

    return expression;
}

Value StyleParser::parseValue(JSVal value) {
    switch (value.GetType()) {
        case rapidjson::kNullType:
        case rapidjson::kFalseType:
            return false;

        case rapidjson::kTrueType:
            return true;

        case rapidjson::kStringType:
            return std::string { value.GetString(), value.GetStringLength() };

        case rapidjson::kNumberType:
            if (value.IsUint64()) return value.GetUint64();
            if (value.IsInt64()) return value.GetInt64();
            return value.GetDouble();

        default:
            return false;
    }
}

std::vector<Value> StyleParser::parseValues(JSVal value) {
    std::vector<Value> values;
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); i++) {
            values.emplace_back(parseValue(replaceConstant(value[i])));
        }
    } else {
        values.emplace_back(parseValue(value));
    }
    return values;
}

void StyleParser::parseRender(JSVal value, std::shared_ptr<StyleLayer> &layer) {
    if (!value.IsObject()) {
        Log::Warning(Event::ParseStyle, "render property of layer '%s' must be an object", layer->id.c_str());
        return;
    }

    StyleBucket &bucket = *layer->bucket;

    switch (layer->type) {
    case StyleLayerType::Fill: {
        StyleBucketFill &render = bucket.render.get<StyleBucketFill>();

        parseRenderProperty<WindingTypeClass>(value, render.winding, "fill-winding");
    } break;

    case StyleLayerType::Line: {
        StyleBucketLine &render = bucket.render.get<StyleBucketLine>();

        parseRenderProperty<CapTypeClass>(value, render.cap, "line-cap");
        parseRenderProperty<JoinTypeClass>(value, render.join, "line-join");
        parseRenderProperty(value, render.miter_limit, "line-miter-limit");
        parseRenderProperty(value, render.round_limit, "line-round-limit");
    } break;

    case StyleLayerType::Symbol: {
        StyleBucketSymbol &render = bucket.render.get<StyleBucketSymbol>();

        parseRenderProperty<PlacementTypeClass>(value, render.placement, "symbol-placement");
        if (render.placement == PlacementType::Line) {
            // Change the default value in case of line placement.
            render.text.rotation_alignment = RotationAlignmentType::Map;
            render.icon.rotation_alignment = RotationAlignmentType::Map;
        }

        parseRenderProperty(value, render.min_distance, "symbol-min-distance");
        parseRenderProperty(value, render.avoid_edges, "symbol-avoid-edges");

        parseRenderProperty(value, render.icon.allow_overlap, "icon-allow-overlap");
        parseRenderProperty(value, render.icon.ignore_placement, "icon-ignore-placement");
        parseRenderProperty(value, render.icon.optional, "icon-optional");
        parseRenderProperty<RotationAlignmentTypeClass>(value, render.icon.rotation_alignment, "icon-rotation-alignment");
        parseRenderProperty(value, render.icon.max_size, "icon-max-size");
        parseRenderProperty(value, render.icon.image, "icon-image");
        parseRenderProperty(value, render.icon.rotate, "icon-rotate");
        parseRenderProperty(value, render.icon.padding, "icon-padding");
        parseRenderProperty(value, render.icon.keep_upright, "icon-keep-upright");
        parseRenderProperty(value, render.icon.offset, "icon-offset");
        parseRenderProperty<TranslateAnchorTypeClass>(value, render.icon.translate_anchor, "icon-translate-anchor");


        parseRenderProperty<RotationAlignmentTypeClass>(value, render.text.rotation_alignment, "text-rotation-alignment");
        parseRenderProperty(value, render.text.field, "text-field");
        parseRenderProperty(value, render.text.font, "text-font");
        parseRenderProperty(value, render.text.max_size, "text-max-size");
        if (parseRenderProperty(value, render.text.max_width, "text-max-width")) {
            render.text.max_width *= 24; // em
        }
        if (parseRenderProperty(value, render.text.line_height, "text-line-height")) {
            render.text.line_height *= 24; // em
        }
        if (parseRenderProperty(value, render.text.letter_spacing, "text-letter-spacing")) {
            render.text.letter_spacing *= 24; // em
        }
        parseRenderProperty<TextJustifyTypeClass>(value, render.text.justify, "text-justify");
        parseRenderProperty<TextHorizontalAlignTypeClass>(value, render.text.horizontal_align, "text-horizontal-align");
        parseRenderProperty<TextVerticalAlignTypeClass>(value, render.text.vertical_align, "text-vertical-align");
        parseRenderProperty(value, render.text.max_angle, "text-max-angle");
        parseRenderProperty(value, render.text.rotate, "text-rotate");
        parseRenderProperty(value, render.text.slant, "text-slant");
        parseRenderProperty(value, render.text.padding, "text-padding");
        parseRenderProperty(value, render.text.keep_upright, "text-keep-upright");
        parseRenderProperty<TextTransformTypeClass>(value, render.text.transform, "text-transform");
        parseRenderProperty(value, render.text.offset, "text-offset");
        parseRenderProperty<TranslateAnchorTypeClass>(value, render.text.translate_anchor, "text-translate-anchor");
        parseRenderProperty(value, render.text.allow_overlap, "text-allow-overlap");
        parseRenderProperty(value, render.text.ignore_placement, "text-ignore-placement");
        parseRenderProperty(value, render.text.optional, "text-optional");
    } break;

    case StyleLayerType::Raster: {
        StyleBucketRaster &render = bucket.render.get<StyleBucketRaster>();

        parseRenderProperty(value, render.size, "raster-size");
        parseRenderProperty(value, render.blur, "raster-blur");
        parseRenderProperty(value, render.buffer, "raster-buffer");
        if (layer->layers) {
            render.prerendered = true;
        }
    } break;

    default:
        // There are no render properties for these layer types.
        break;
    }
}

void StyleParser::parseSprite(JSVal value) {
    if (value.IsString()) {
        sprite = { value.GetString(), value.GetStringLength() };
    }
}

void StyleParser::parseGlyphURL(JSVal value) {
    if (value.IsString()) {
        glyph_url = { value.GetString(), value.GetStringLength() };
    }
}


}
