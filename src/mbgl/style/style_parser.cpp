#include <mbgl/style/style_source.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/platform/log.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <algorithm>

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
            unsigned int int_value = property.GetUint();
            if (int_value > std::numeric_limits<uint16_t>::max()) {
                Log::Warning(Event::ParseStyle, "values for %s that are larger than %d are not supported", name, std::numeric_limits<uint16_t>::max());
                return false;
            }

            target = int_value;
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
            SourceInfo& info = sources.emplace(name, std::make_shared<StyleSource>()).first->second->info;

            parseRenderProperty<SourceTypeClass>(itr->value, info.type, "type");
            parseRenderProperty(itr->value, info.url, "url");
            parseRenderProperty(itr->value, info.tile_size, "tileSize");
            info.parseTileJSONProperties(itr->value);
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

std::tuple<bool,std::vector<float>> parseFloatArray(JSVal value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "dasharray value must be an array of numbers");
        return std::tuple<bool, std::vector<float>> { false, std::vector<float>() };
    }

    std::vector<float> vec;
    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        JSVal part = value[i];
        if (!part.IsNumber()) {
            Log::Warning(Event::ParseStyle, "dasharray value must be an array of numbers");
            return std::tuple<bool, std::vector<float>> { false, std::vector<float>() };
        }
        vec.push_back(part.GetDouble());
    }
    return std::tuple<bool, std::vector<float>> { true, vec };
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

template <>
Faded<std::vector<float>> StyleParser::parseFunctionArgument(JSVal value) {
    Faded<std::vector<float>> parsed;
    JSVal rvalue = replaceConstant(value);
    parsed.low = std::get<1>(parseFloatArray(rvalue));
    return parsed;
}

template <>
Faded<std::string> StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsString()) {
        Faded<std::string> parsed;
        parsed.low = { value.GetString(), value.GetStringLength() };
        return parsed;
    } else {
        Log::Warning(Event::ParseStyle, "function argument must be a string");
        return {};
    }
}

template <typename T>
std::tuple<bool, std::vector<std::pair<float, T>>> StyleParser::parseStops(JSVal value_stops) {

    if (!value_stops.IsArray()) {
        Log::Warning(Event::ParseStyle, "stops function must specify a stops array");
        return std::tuple<bool, std::vector<std::pair<float, T>>> { false, {}};
    }

    std::vector<std::pair<float, T>> stops;

    for (rapidjson::SizeType i = 0; i < value_stops.Size(); ++i) {
        JSVal stop = value_stops[i];
        if (stop.IsArray()) {
            if (stop.Size() != 2) {
                Log::Warning(Event::ParseStyle, "stop must have zoom level and value specification");
                return std::tuple<bool, std::vector<std::pair<float, T>>> { false, {}};
            }

            JSVal z = stop[rapidjson::SizeType(0)];
            if (!z.IsNumber()) {
                Log::Warning(Event::ParseStyle, "zoom level in stop must be a number");
                return std::tuple<bool, std::vector<std::pair<float, T>>> { false, {}};
            }

            stops.emplace_back(z.GetDouble(), parseFunctionArgument<T>(stop[rapidjson::SizeType(1)]));
        } else {
            Log::Warning(Event::ParseStyle, "function argument must be a numeric value");
            return std::tuple<bool, std::vector<std::pair<float, T>>> { false, {}};
        }
    }
    return { true, stops };
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

    auto stops = parseStops<T>(value["stops"]);

    if (!std::get<0>(stops)) {
        return std::tuple<bool, Function<T>> { false, ConstantFunction<T>(T()) };
    }

    return std::tuple<bool, Function<T>> { true, StopsFunction<T>(std::get<1>(stops), base) };
}

template <typename T> inline std::chrono::duration<float> defaultDurationValue() { return std::chrono::duration<float>(300.0f); }

template <typename T>
std::tuple<bool, FadedStopsFunction<T>> StyleParser::parseFadedStopsFunction(JSVal value) {
    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return std::tuple<bool, FadedStopsFunction<T>> { false, {} };
    }

    std::chrono::duration<float> duration = defaultDurationValue<T>();

    if (value.HasMember("duration")) {
        JSVal value_duration = value["duration"];
        if (value_duration.IsNumber()) {
            duration = static_cast<std::chrono::duration<float>>(value_duration.GetDouble());
        } else {
            Log::Warning(Event::ParseStyle, "duration must be numeric");
        }
    }

    auto stops = parseStops<T>(value["stops"]);

    if (!std::get<0>(stops)) {
        return std::tuple<bool, FadedStopsFunction<T>> { false, {} };
    }

    return std::tuple<bool, FadedStopsFunction<T>> { true, { std::get<1>(stops), duration } };
}

template <typename T>
bool StyleParser::setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass) {
    auto res = parseProperty<T>(value, property_name);
    if (std::get<0>(res)) {
        klass.set(key, std::get<1>(res));
    }
    return std::get<0>(res);
}

template<typename T>
void StyleParser::parseVisibility(StyleBucket &bucket, JSVal value) {
    if (!value.HasMember("visibility")) {
        return;
    } else if (!value["visibility"].IsString()) {
        Log::Warning(Event::ParseStyle, "value of 'visibility' must be a string");
        bucket.visibility = VisibilityType::Visible;
        return;
    }
    bucket.visibility = VisibilityTypeClass({ value["visibility"].GetString(), value["visibility"].GetStringLength() });
}

template<typename T>
bool StyleParser::parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    } else {
        return setProperty<T>(replaceConstant(value[property_name]), property_name, key, klass);
    }
}

template<> std::tuple<bool, std::string> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, std::string> { false, std::string() };
    }

    return std::tuple<bool, std::string> { true, { value.GetString(), value.GetStringLength() } };
}

template<> std::tuple<bool, bool> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (!value.IsBool()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a boolean", property_name);
        return std::tuple<bool, bool> { false, true };
    }

    return std::tuple<bool, bool> { true, value.GetBool() };
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

template<> std::tuple<bool, CapType> StyleParser::parseProperty<CapType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, CapType> { false, CapType::Butt };
    }

    return std::tuple<bool, CapType> { true, CapTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, JoinType> StyleParser::parseProperty<JoinType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, JoinType> { false, JoinType::Miter };
    }

    return std::tuple<bool, JoinType> { true, JoinTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, PlacementType> StyleParser::parseProperty<PlacementType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, PlacementType> { false, PlacementType::Point };
    }

    return std::tuple<bool, PlacementType> { true, PlacementTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, TextAnchorType> StyleParser::parseProperty<TextAnchorType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, TextAnchorType> { false, TextAnchorType::Center };
    }

    return std::tuple<bool, TextAnchorType> { true, TextAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, TextJustifyType> StyleParser::parseProperty<TextJustifyType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, TextJustifyType> { false, TextJustifyType::Center };
    }

    return std::tuple<bool, TextJustifyType> { true, TextJustifyTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, TextTransformType> StyleParser::parseProperty<TextTransformType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, TextTransformType> { false, TextTransformType::None };
    }

    return std::tuple<bool, TextTransformType> { true, TextTransformTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, RotationAlignmentType> StyleParser::parseProperty<RotationAlignmentType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return std::tuple<bool, RotationAlignmentType> { false, RotationAlignmentType::Map };
    }

    return std::tuple<bool, RotationAlignmentType> { true, RotationAlignmentTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> std::tuple<bool, PropertyTransition> StyleParser::parseProperty(JSVal value, const char */*property_name*/) {
    PropertyTransition transition;
    if (value.IsObject()) {
        if (value.HasMember("duration") && value["duration"].IsNumber()) {
            transition.duration = std::chrono::milliseconds(value["duration"].GetUint());
        }
        if (value.HasMember("delay") && value["delay"].IsNumber()) {
            transition.delay = std::chrono::milliseconds(value["delay"].GetUint());
        }
    }

    if (transition.duration == std::chrono::steady_clock::duration::zero() && transition.delay == std::chrono::steady_clock::duration::zero()) {
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

template<> std::tuple<bool, FadedStopsFunction<Faded<std::vector<float>>>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (value.IsObject()) {
        return parseFadedStopsFunction<Faded<std::vector<float>>>(value);
    } else if (value.IsArray()) {
        Faded<std::vector<float>> parsed;
        std::tuple<bool, std::vector<float>> floatarray = parseFloatArray(value);
        parsed.low = std::get<1>(floatarray);
        return std::tuple<bool, FadedStopsFunction<Faded<std::vector<float>>>> { std::get<0>(floatarray),  parsed };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be an array of numbers, or a number array function", property_name);
        return std::tuple<bool, FadedStopsFunction<Faded<std::vector<float>>>> { false, {} };
    }
}

template<> std::tuple<bool, FadedStopsFunction<Faded<std::string>>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (value.IsObject()) {
        return parseFadedStopsFunction<Faded<std::string>>(value);
    } else if (value.IsString()) {
        Faded<std::string> parsed;
        parsed.low = { value.GetString(), value.GetStringLength() };
        return std::tuple<bool, FadedStopsFunction<Faded<std::string>>> { true,  parsed };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be string or a string function", property_name);
        return std::tuple<bool, FadedStopsFunction<Faded<std::string>>> { false, {} };
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
        std::unique_ptr<StyleLayerGroup> group = util::make_unique<StyleLayerGroup>();
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            util::ptr<StyleLayer> layer = createLayer(value[i]);
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

util::ptr<StyleLayer> StyleParser::createLayer(JSVal value) {
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

        // Parse paints already, as they can't be inherited anyway.
        std::map<ClassID, ClassProperties> paints;
        parsePaints(value, paints);

        util::ptr<StyleLayer> layer = std::make_shared<StyleLayer>(
            layer_id, std::move(paints));

        // Store the layer ID so we can reference it later.
        layers.emplace(layer_id, std::pair<JSVal, util::ptr<StyleLayer>> { value, layer });

        return layer;
    } else {
        Log::Warning(Event::ParseStyle, "layer must be an object");
        return nullptr;
    }
}

void StyleParser::parseLayers() {
    for (std::pair<const std::string, std::pair<JSVal, util::ptr<StyleLayer>>> &pair : layers) {
        parseLayer(pair.second);
    }
}

void StyleParser::parseLayer(std::pair<JSVal, util::ptr<StyleLayer>> &pair) {
    JSVal value = pair.first;
    util::ptr<StyleLayer> &layer = pair.second;

    if (value.HasMember("type")) {
        JSVal type = value["type"];
        if (!type.IsString()) {
            Log::Warning(Event::ParseStyle, "layer type of '%s' must be a string", layer->id.c_str());
        } else {
            layer->type = StyleLayerTypeClass(std::string { type.GetString(), type.GetStringLength() });
        }
    }

    if (layer->bucket) {
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

void StyleParser::parsePaints(JSVal value, std::map<ClassID, ClassProperties> &paints) {
    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const std::string name { itr->name.GetString(), itr->name.GetStringLength() };

        if (name == "paint") {
            parsePaint(replaceConstant(itr->value), paints[ClassID::Default]);
        } else if (name.compare(0, 6, "paint.") == 0 && name.length() > 6) {
            const ClassID class_id = ClassDictionary::Get().lookup(name.substr(6));
            parsePaint(replaceConstant(itr->value), paints[class_id]);
        }
    }
}

void StyleParser::parsePaint(JSVal value, ClassProperties &klass) {
    using Key = PropertyKey;

    parseOptionalProperty<Function<bool>>("fill-antialias", Key::FillAntialias, klass, value);
    parseOptionalProperty<Function<float>>("fill-opacity", Key::FillOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("fill-opacity-transition", Key::FillOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("fill-color", Key::FillColor, klass, value);
    parseOptionalProperty<PropertyTransition>("fill-color-transition", Key::FillColor, klass, value);
    parseOptionalProperty<Function<Color>>("fill-outline-color", Key::FillOutlineColor, klass, value);
    parseOptionalProperty<PropertyTransition>("fill-outline-color-transition", Key::FillOutlineColor, klass, value);
    parseOptionalProperty<Function<float>>("fill-translate", { Key::FillTranslateX, Key::FillTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("fill-translate-transition", Key::FillTranslate, klass, value);
    parseOptionalProperty<TranslateAnchorType>("fill-translate-anchor", Key::FillTranslateAnchor, klass, value);
    parseOptionalProperty<FadedStopsFunction<Faded<std::string>>>("fill-image", Key::FillImage, klass, value);

    parseOptionalProperty<Function<float>>("line-opacity", Key::LineOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("line-opacity-transition", Key::LineOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("line-color", Key::LineColor, klass, value);
    parseOptionalProperty<PropertyTransition>("line-color-transition", Key::LineColor, klass, value);
    parseOptionalProperty<Function<float>>("line-translate", { Key::LineTranslateX, Key::LineTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("line-translate-transition", Key::LineTranslate, klass, value);
    parseOptionalProperty<TranslateAnchorType>("line-translate-anchor", Key::LineTranslateAnchor, klass, value);
    parseOptionalProperty<Function<float>>("line-width", Key::LineWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("line-width-transition", Key::LineWidth, klass, value);
    parseOptionalProperty<Function<float>>("line-gap-width", Key::LineGapWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("line-gap-width-transition", Key::LineGapWidth, klass, value);
    parseOptionalProperty<Function<float>>("line-blur", Key::LineBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("line-blur-transition", Key::LineBlur, klass, value);
    parseOptionalProperty<FadedStopsFunction<Faded<std::vector<float>>>>("line-dasharray", Key::LineDashArray, klass, value);
    parseOptionalProperty<FadedStopsFunction<Faded<std::string>>>("line-image", Key::LineImage, klass, value);

    parseOptionalProperty<Function<float>>("icon-opacity", Key::IconOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-opacity-transition", Key::IconOpacity, klass, value);
    parseOptionalProperty<Function<float>>("icon-rotate", Key::IconRotate, klass, value);
    parseOptionalProperty<Function<float>>("icon-size", Key::IconSize, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-size-transition", Key::IconSize, klass, value);
    parseOptionalProperty<Function<Color>>("icon-color", Key::IconColor, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-color-transition", Key::IconColor, klass, value);
    parseOptionalProperty<Function<Color>>("icon-halo-color", Key::IconHaloColor, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-halo-color-transition", Key::IconHaloColor, klass, value);
    parseOptionalProperty<Function<float>>("icon-halo-width", Key::IconHaloWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-halo-width-transition", Key::IconHaloWidth, klass, value);
    parseOptionalProperty<Function<float>>("icon-halo-blur", Key::IconHaloBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-halo-blur-transition", Key::IconHaloBlur, klass, value);
    parseOptionalProperty<Function<float>>("icon-translate", { Key::IconTranslateX, Key::IconTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-translate-transition", Key::IconTranslate, klass, value);
    parseOptionalProperty<TranslateAnchorType>("icon-translate-anchor", Key::IconTranslateAnchor, klass, value);

    parseOptionalProperty<Function<float>>("text-opacity", Key::TextOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("text-opacity-transition", Key::TextOpacity, klass, value);
    parseOptionalProperty<Function<float>>("text-size", Key::TextSize, klass, value);
    parseOptionalProperty<PropertyTransition>("text-size-transition", Key::TextSize, klass, value);
    parseOptionalProperty<Function<Color>>("text-color", Key::TextColor, klass, value);
    parseOptionalProperty<PropertyTransition>("text-color-transition", Key::TextColor, klass, value);
    parseOptionalProperty<Function<Color>>("text-halo-color", Key::TextHaloColor, klass, value);
    parseOptionalProperty<PropertyTransition>("text-halo-color-transition", Key::TextHaloColor, klass, value);
    parseOptionalProperty<Function<float>>("text-halo-width", Key::TextHaloWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("text-halo-width-transition", Key::TextHaloWidth, klass, value);
    parseOptionalProperty<Function<float>>("text-halo-blur", Key::TextHaloBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("text-halo-blur-transition", Key::TextHaloBlur, klass, value);
    parseOptionalProperty<Function<float>>("text-translate", { Key::TextTranslateX, Key::TextTranslateY }, klass, value);
    parseOptionalProperty<PropertyTransition>("text-translate-transition", Key::TextTranslate, klass, value);
    parseOptionalProperty<TranslateAnchorType>("text-translate-anchor", Key::TextTranslateAnchor, klass, value);

    parseOptionalProperty<Function<float>>("raster-opacity", Key::RasterOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-opacity-transition", Key::RasterOpacity, klass, value);
    parseOptionalProperty<Function<float>>("raster-hue-rotate", Key::RasterHueRotate, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-hue-rotate-transition", Key::RasterHueRotate, klass, value);
    parseOptionalProperty<Function<float>>("raster-brightness", { Key::RasterBrightnessLow, Key::RasterBrightnessHigh }, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-brightness-transition", Key::RasterBrightness, klass, value);
    parseOptionalProperty<Function<float>>("raster-saturation", Key::RasterSaturation, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-saturation-transition", Key::RasterSaturation, klass, value);
    parseOptionalProperty<Function<float>>("raster-contrast", Key::RasterContrast, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-contrast-transition", Key::RasterContrast, klass, value);
    parseOptionalProperty<Function<float>>("raster-fade-duration", Key::RasterFade, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-fade-duration-transition", Key::RasterFade, klass, value);

    parseOptionalProperty<Function<float>>("background-opacity", Key::BackgroundOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("background-color", Key::BackgroundColor, klass, value);
    parseOptionalProperty<FadedStopsFunction<Faded<std::string>>>("background-image", Key::BackgroundImage, klass, value);
}

void StyleParser::parseLayout(JSVal value, util::ptr<StyleBucket> &bucket) {
    using Key = PropertyKey;

    parseVisibility<VisibilityType>(*bucket, value);

    parseOptionalProperty<CapType>("line-cap", Key::LineCap, bucket->layout, value);
    parseOptionalProperty<JoinType>("line-join", Key::LineJoin, bucket->layout, value);
    parseOptionalProperty<Function<float>>("line-miter-limit", Key::LineMiterLimit, bucket->layout, value);
    parseOptionalProperty<Function<float>>("line-round-limit", Key::LineRoundLimit, bucket->layout, value);

    parseOptionalProperty<PlacementType>("symbol-placement", Key::SymbolPlacement, bucket->layout, value);
    parseOptionalProperty<Function<float>>("symbol-min-distance", Key::SymbolMinDistance, bucket->layout, value);
    parseOptionalProperty<bool>("symbol-avoid-edges", Key::SymbolAvoidEdges, bucket->layout, value);
    parseOptionalProperty<bool>("icon-allow-overlap", Key::IconAllowOverlap, bucket->layout, value);
    parseOptionalProperty<bool>("icon-ignore-placement", Key::IconIgnorePlacement, bucket->layout, value);
    parseOptionalProperty<bool>("icon-optional", Key::IconOptional, bucket->layout, value);
    parseOptionalProperty<RotationAlignmentType>("icon-rotation-alignment", Key::IconRotationAlignment, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-max-size", Key::IconMaxSize, bucket->layout, value);
    parseOptionalProperty<std::string>("icon-image", Key::IconImage, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-rotate", Key::IconRotate, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-padding", Key::IconPadding, bucket->layout, value);
    parseOptionalProperty<bool>("icon-keep-upright", Key::IconKeepUpright, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-offset", { Key::IconOffsetX, Key::IconOffsetY }, bucket->layout, value);
    parseOptionalProperty<RotationAlignmentType>("text-rotation-alignment", Key::TextRotationAlignment, bucket->layout, value);
    parseOptionalProperty<std::string>("text-field", Key::TextField, bucket->layout, value);
    parseOptionalProperty<std::string>("text-font", Key::TextFont, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-max-size", Key::TextMaxSize, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-max-width", Key::TextMaxWidth, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-line-height", Key::TextLineHeight, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-letter-spacing", Key::TextLetterSpacing, bucket->layout, value);
    parseOptionalProperty<TextJustifyType>("text-justify", Key::TextJustify, bucket->layout, value);
    parseOptionalProperty<TextAnchorType>("text-anchor", Key::TextAnchor, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-max-angle", Key::TextMaxAngle, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-rotate", Key::TextRotate, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-padding", Key::TextPadding, bucket->layout, value);
    parseOptionalProperty<bool>("text-keep-upright", Key::TextKeepUpright, bucket->layout, value);
    parseOptionalProperty<TextTransformType>("text-transform", Key::TextTransform, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-offset", { Key::TextOffsetX, Key::TextOffsetY }, bucket->layout, value);
    parseOptionalProperty<bool>("text-allow-overlap", Key::TextAllowOverlap, bucket->layout, value);
    parseOptionalProperty<bool>("text-ignore-placement", Key::TextIgnorePlacement, bucket->layout, value);
    parseOptionalProperty<bool>("text-optional", Key::TextOptional, bucket->layout, value);

}

void StyleParser::parseReference(JSVal value, util::ptr<StyleLayer> &layer) {
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

    util::ptr<StyleLayer> reference = it->second.second;
    layer->type = reference->type;
    layer->bucket = reference->bucket;
}

#pragma mark - Parse Bucket

void StyleParser::parseBucket(JSVal value, util::ptr<StyleLayer> &layer) {
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
        layer->bucket->filter = parseFilterExpression(value_filter);
    }

    if (value.HasMember("layout")) {
        JSVal value_render = replaceConstant(value["layout"]);
        parseLayout(value_render, layer->bucket);
    }

    if (value.HasMember("minzoom")) {
        JSVal min_zoom = value["minzoom"];
        if (min_zoom.IsNumber()) {
            layer->bucket->min_zoom = min_zoom.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "minzoom of layer %s must be numeric", layer->id.c_str());
        }
    }

    if (value.HasMember("maxzoom")) {
        JSVal max_zoom = value["maxzoom"];
        if (max_zoom.IsNumber()) {
            layer->bucket->min_zoom = max_zoom.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "maxzoom of layer %s must be numeric", layer->id.c_str());
        }
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
