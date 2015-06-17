#include <mbgl/style/style_parser.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/annotation.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>
#include <csscolorparser/csscolorparser.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <algorithm>

namespace mbgl {

using JSVal = const rapidjson::Value&;

StyleParser::StyleParser(MapData& data_)
    : data(data_) {
}

void StyleParser::parse(JSVal document) {
    if (document.HasMember("version")) {
        version = document["version"].GetInt();
        if (version != 8) {
            Log::Warning(Event::ParseStyle, "current renderer implementation only supports style spec version 8; using an outdated style will cause rendering errors");
        }
    }

    if (document.HasMember("constants")) {
        parseConstants(document["constants"]);
    }

    if (document.HasMember("sources")) {
        parseSources(document["sources"]);
    }

    if (document.HasMember("layers")) {
        parseLayers(document["layers"]);

        // create shape annotations source
        const std::string& shapeID = AnnotationManager::ShapeLayerID;

        std::unique_ptr<Source> shapeAnnotationsSource = std::make_unique<Source>();
        shapeAnnotationsSource->info.type = SourceType::Annotations;
        shapeAnnotationsSource->info.source_id = shapeID;
        sourcesMap.emplace(shapeID, shapeAnnotationsSource.get());
        sources.emplace_back(std::move(shapeAnnotationsSource));

        // create point annotations layer
        const std::string& pointID = AnnotationManager::PointLayerID;

        std::map<ClassID, ClassProperties> pointPaints;
        util::ptr<StyleLayer> pointAnnotationsLayer = std::make_shared<StyleLayer>(pointID, std::move(pointPaints));
        pointAnnotationsLayer->type = StyleLayerType::Symbol;
        layersMap.emplace(pointID, std::pair<JSVal, util::ptr<StyleLayer>> { JSVal(pointID), pointAnnotationsLayer });
        layers.emplace_back(pointAnnotationsLayer);

        // create point annotations symbol bucket
        util::ptr<StyleBucket> pointBucket = std::make_shared<StyleBucket>(pointAnnotationsLayer->type);
        pointBucket->name = pointAnnotationsLayer->id;
        pointBucket->source = pointID;
        pointBucket->source_layer = pointAnnotationsLayer->id;

        // build up point annotations style
        rapidjson::Document d;
        rapidjson::Value iconImage(rapidjson::kObjectType);
        iconImage.AddMember("icon-image", "{sprite}", d.GetAllocator());
        parseLayout(iconImage, pointBucket);
        rapidjson::Value iconOverlap(rapidjson::kObjectType);
        iconOverlap.AddMember("icon-allow-overlap", true, d.GetAllocator());
        parseLayout(iconOverlap, pointBucket);

        // create point annotations source & connect to bucket & layer
        std::unique_ptr<Source> pointAnnotationsSource = std::make_unique<Source>();
        pointAnnotationsSource->info.type = SourceType::Annotations;
        pointAnnotationsSource->info.source_id = pointID;
        pointAnnotationsLayer->bucket = pointBucket;
        sourcesMap.emplace(pointID, pointAnnotationsSource.get());
        sources.emplace_back(std::move(pointAnnotationsSource));
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

template<> StyleParser::Status StyleParser::parseRenderProperty(JSVal value, bool &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsBool()) {
            target = property.GetBool();
            return StyleParserSuccess;
        } else {
            Log::Warning(Event::ParseStyle, "'%s' must be a boolean", name);
        }
    }
    return StyleParserFailure;
}


template<> StyleParser::Status StyleParser::parseRenderProperty(JSVal value, std::string &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsString()) {
            target = { property.GetString(), property.GetStringLength() };
            return StyleParserSuccess;
        } else {
            Log::Warning(Event::ParseStyle, "'%s' must be a string", name);
        }
    }
    return StyleParserFailure;
}

template<> StyleParser::Status StyleParser::parseRenderProperty(JSVal value, float &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsNumber()) {
            target = property.GetDouble();
            return StyleParserSuccess;
        } else {
            Log::Warning(Event::ParseStyle, "'%s' must be a number", name);
        }
    }
    return StyleParserFailure;
}

template<> StyleParser::Status StyleParser::parseRenderProperty(JSVal value, uint16_t &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsUint()) {
            unsigned int int_value = property.GetUint();
            if (int_value > std::numeric_limits<uint16_t>::max()) {
                Log::Warning(Event::ParseStyle, "values for %s that are larger than %d are not supported", name, std::numeric_limits<uint16_t>::max());
                return StyleParserFailure;
            }

            target = int_value;
            return StyleParserSuccess;
        } else {
            Log::Warning(Event::ParseStyle, "%s must be an unsigned integer", name);
        }
    }
    return StyleParserFailure;
}

template<> StyleParser::Status StyleParser::parseRenderProperty(JSVal value, int32_t &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsInt()) {
            target = property.GetInt();
            return StyleParserSuccess;
        } else {
            Log::Warning(Event::ParseStyle, "%s must be an integer", name);
        }
    }
    return StyleParserFailure;
}

template<> StyleParser::Status StyleParser::parseRenderProperty(JSVal value, vec2<float> &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsArray()) {
            if (property.Size() >= 2) {
                target.x = property[(rapidjson::SizeType)0].GetDouble();
                target.y = property[(rapidjson::SizeType)1].GetDouble();
                return StyleParserSuccess;
            } else {
                Log::Warning(Event::ParseStyle, "%s must have at least two members", name);
            }
        } else {
            Log::Warning(Event::ParseStyle, "%s must be an array of numbers", name);
        }
    }
    return StyleParserFailure;
}

template<typename Parser, typename T>
StyleParser::Status StyleParser::parseRenderProperty(JSVal value, T &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsString()) {
            target = Parser({ property.GetString(), property.GetStringLength() });
            return StyleParserSuccess;
        } else {
            Log::Warning(Event::ParseStyle, "%s must have one of the enum values", name);
        }
    }
    return StyleParserFailure;
}


#pragma mark - Parse Sources

void StyleParser::parseSources(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            std::unique_ptr<Source> source = std::make_unique<Source>();
            parseRenderProperty<SourceTypeClass>(itr->value, source->info.type, "type");
            parseRenderProperty(itr->value, source->info.url, "url");
            parseRenderProperty(itr->value, source->info.tile_size, "tileSize");
            source->info.source_id = name;
            source->info.parseTileJSONProperties(itr->value);
            sourcesMap.emplace(name, source.get());
            sources.emplace_back(std::move(source));
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

StyleParser::Result<std::vector<float>> StyleParser::parseFloatArray(JSVal value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "dasharray value must be an array of numbers");
        return Result<std::vector<float>> { StyleParserFailure, std::vector<float>() };
    }

    std::vector<float> vec;
    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        JSVal part = replaceConstant(value[i]);
        if (!part.IsNumber()) {
            Log::Warning(Event::ParseStyle, "dasharray value must be an array of numbers");
            return Result<std::vector<float>> { StyleParserFailure, std::vector<float>() };
        }
        vec.push_back(part.GetDouble());
    }
    return Result<std::vector<float>> { StyleParserSuccess, vec };
}

template <>
StyleParser::Result<std::array<float, 2>> StyleParser::parseProperty(JSVal value, const char*) {
    if (value.IsArray() && value.Size() == 2 &&
            value[rapidjson::SizeType(0)].IsNumber() &&
            value[rapidjson::SizeType(1)].IsNumber()) {

        float first = value[rapidjson::SizeType(0)].GetDouble();
        float second = value[rapidjson::SizeType(1)].GetDouble();
        return Result<std::array<float, 2>> { StyleParserSuccess, {{ first, second }} };
    } else {
        Log::Warning(Event::ParseStyle, "value must be array of two numbers");
        return Result<std::array<float, 2>> { StyleParserFailure, {{ 0.0f, 0.0f }} };
    }
}

template <>
StyleParser::Result<float> StyleParser::parseProperty(JSVal value, const char* property_name) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsNumber()) {
        return Result<float> { StyleParserSuccess, rvalue.GetDouble() };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a number, or a number function", property_name);
        return Result<float> { StyleParserFailure, 0.0f };
    }
}

template <>
StyleParser::Result<Color> StyleParser::parseProperty(JSVal value, const char*) {
    JSVal rvalue = replaceConstant(value);
    return Result<Color> { StyleParserSuccess, parseColor(rvalue) };
}

template <>
StyleParser::Result<Faded<std::vector<float>>> StyleParser::parseProperty(JSVal value, const char*) {
    Faded<std::vector<float>> parsed;
    JSVal rvalue = replaceConstant(value);
    parsed.to = std::get<1>(parseFloatArray(rvalue));
    return Result<Faded<std::vector<float>>> { StyleParserSuccess, parsed };
}

template <>
StyleParser::Result<Faded<std::string>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    JSVal rvalue = replaceConstant(value);
    Faded<std::string> parsed;
    if (rvalue.IsString()) {
        parsed.to = { value.GetString(), value.GetStringLength() };
        return Result<Faded<std::string>> { StyleParserSuccess, parsed };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string, or a string function", property_name);
        return Result<Faded<std::string>> { StyleParserFailure, parsed };
    }
}

template <typename T>
StyleParser::Result<std::vector<std::pair<float, T>>> StyleParser::parseStops(JSVal value_stops, const char *property_name) {

    if (!value_stops.IsArray()) {
        Log::Warning(Event::ParseStyle, "stops function must specify a stops array");
        return Result<std::vector<std::pair<float, T>>> { StyleParserFailure, {}};
    }

    std::vector<std::pair<float, T>> stops;

    for (rapidjson::SizeType i = 0; i < value_stops.Size(); ++i) {
        JSVal stop = value_stops[i];
        if (stop.IsArray()) {
            if (stop.Size() != 2) {
                Log::Warning(Event::ParseStyle, "stop must have zoom level and value specification");
                return Result<std::vector<std::pair<float, T>>> { StyleParserFailure, {}};
            }

            JSVal z = stop[rapidjson::SizeType(0)];
            if (!z.IsNumber()) {
                Log::Warning(Event::ParseStyle, "zoom level in stop must be a number");
                return Result<std::vector<std::pair<float, T>>> { StyleParserFailure, {}};
            }

            stops.emplace_back(z.GetDouble(), std::get<1>(parseProperty<T>(replaceConstant(stop[rapidjson::SizeType(1)]), property_name)));
        } else {
            Log::Warning(Event::ParseStyle, "function argument must be a numeric value");
            return Result<std::vector<std::pair<float, T>>> { StyleParserFailure, {}};
        }
    }
    return Result<std::vector<std::pair<float, T>>>(StyleParserSuccess, stops);
}

template <typename T>
StyleParser::Result<Function<T>> StyleParser::parseFunction(JSVal value, const char *property_name) {
    if (!value.IsObject()) {
        return parseProperty<T>(value, property_name);
    }

    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return Result<Function<T>> { StyleParserFailure, ConstantFunction<T>(T()) };
    }

    float base = 1.0f;

    if (value.HasMember("base")) {
        JSVal value_base = value["base"];
        if (value_base.IsNumber()) {
            base = value_base.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "base must be numeric");
        }
    }

    auto stops = parseStops<T>(value["stops"], property_name);

    if (!std::get<0>(stops)) {
        return Result<Function<T>> { StyleParserFailure, ConstantFunction<T>(T()) };
    }

    return Result<Function<T>> { StyleParserSuccess, StopsFunction<T>(std::get<1>(stops), base) };
}

template <typename T>
StyleParser::Result<PiecewiseConstantFunction<T>> StyleParser::parsePiecewiseConstantFunction(JSVal value, Duration duration) {
    if (!value.HasMember("stops")) {
        Log::Warning(Event::ParseStyle, "function must specify a function type");
        return Result<PiecewiseConstantFunction<T>> { StyleParserFailure, { {}, duration } };
    }

    auto stops = parseStops<T>(value["stops"], "");

    if (!std::get<0>(stops)) {
        return Result<PiecewiseConstantFunction<T>> { StyleParserFailure, { {}, duration } };
    }

    return Result<PiecewiseConstantFunction<T>> { StyleParserSuccess, { std::get<1>(stops), duration } };
}

template <typename T>
StyleParser::Status StyleParser::setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass) {
    auto res = parseProperty<T>(value, property_name);
    if (std::get<0>(res)) {
        klass.set(key, std::get<1>(res));
    }
    return std::get<0>(res);
}

template <typename T>
StyleParser::Status StyleParser::setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass, JSVal transition) {
    auto res = parseProperty<T>(value, property_name, transition);
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
StyleParser::Status StyleParser::parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return StyleParserFailure;
    } else {
        return setProperty<T>(replaceConstant(value[property_name]), property_name, key, klass);
    }
}

template<typename T>
StyleParser::Status StyleParser::parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value, const char *transition_name) {
    if (!value.HasMember(property_name)) {
        return StyleParserFailure;
    } else {
        if (value.HasMember(transition_name)) {
            return setProperty<T>(replaceConstant(value[property_name]), property_name, key, klass, value[transition_name]);
        } else {
            JSVal val = JSVal(rapidjson::kObjectType);
            return setProperty<T>(replaceConstant(value[property_name]), property_name, key, klass, val);
        }
    }
}

template<> StyleParser::Result<std::string> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (std::string { "text-font" } == property_name) {
        if (!value.IsArray()) {
            Log::Warning(Event::ParseStyle, "value of '%s' must be an array of strings", property_name);
            return Result<std::string> { StyleParserFailure, std::string() };
        } else {
            std::string result = "";
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
                JSVal stop = value[i];
                if (stop.IsString()) {
                    result += stop.GetString();
                    if (i < value.Size()-1) {
                        result += ",";
                    }
                } else {
                    Log::Warning(Event::ParseStyle, "text-font members must be strings");
                    return Result<std::string> { StyleParserFailure, {} };
                }
            }
            return Result<std::string> { StyleParserSuccess, result };
        }
    } else if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<std::string> { StyleParserFailure, std::string() };
    } else {
        return Result<std::string> { StyleParserSuccess, { value.GetString(), value.GetStringLength() } };
    }
}

template<> StyleParser::Result<bool> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (!value.IsBool()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a boolean", property_name);
        return Result<bool> { StyleParserFailure, StyleParserSuccess };
    }

    return Result<bool> { StyleParserSuccess, value.GetBool() };
}

template<> StyleParser::Result<TranslateAnchorType> StyleParser::parseProperty(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<TranslateAnchorType> { StyleParserFailure, TranslateAnchorType::Map };
    }

    return Result<TranslateAnchorType> { StyleParserSuccess, TranslateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<RotateAnchorType> StyleParser::parseProperty<RotateAnchorType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<RotateAnchorType> { StyleParserFailure, RotateAnchorType::Map };
    }

    return Result<RotateAnchorType> { StyleParserSuccess, RotateAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<CapType> StyleParser::parseProperty<CapType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<CapType> { StyleParserFailure, CapType::Butt };
    }

    return Result<CapType> { StyleParserSuccess, CapTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<JoinType> StyleParser::parseProperty<JoinType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<JoinType> { StyleParserFailure, JoinType::Miter };
    }

    return Result<JoinType> { StyleParserSuccess, JoinTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<PlacementType> StyleParser::parseProperty<PlacementType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<PlacementType> { StyleParserFailure, PlacementType::Point };
    }

    return Result<PlacementType> { StyleParserSuccess, PlacementTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<TextAnchorType> StyleParser::parseProperty<TextAnchorType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<TextAnchorType> { StyleParserFailure, TextAnchorType::Center };
    }

    return Result<TextAnchorType> { StyleParserSuccess, TextAnchorTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<TextJustifyType> StyleParser::parseProperty<TextJustifyType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<TextJustifyType> { StyleParserFailure, TextJustifyType::Center };
    }

    return Result<TextJustifyType> { StyleParserSuccess, TextJustifyTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<TextTransformType> StyleParser::parseProperty<TextTransformType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<TextTransformType> { StyleParserFailure, TextTransformType::None };
    }

    return Result<TextTransformType> { StyleParserSuccess, TextTransformTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<RotationAlignmentType> StyleParser::parseProperty<RotationAlignmentType>(JSVal value, const char *property_name) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "value of '%s' must be a string", property_name);
        return Result<RotationAlignmentType> { StyleParserFailure, RotationAlignmentType::Map };
    }

    return Result<RotationAlignmentType> { StyleParserSuccess, RotationAlignmentTypeClass({ value.GetString(), value.GetStringLength() }) };
}

template<> StyleParser::Result<PropertyTransition> StyleParser::parseProperty(JSVal value, const char */*property_name*/) {
    PropertyTransition transition { data.getDefaultTransitionDuration(), data.getDefaultTransitionDelay() };
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
            return Result<PropertyTransition> { StyleParserFailure, std::move(transition) };
        }
    }
    return Result<PropertyTransition> { StyleParserSuccess, std::move(transition) };
}

template<> StyleParser::Result<Function<std::array<float, 2>>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<std::array<float, 2>>(value, property_name);
}

template<> StyleParser::Result<Function<std::string>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<std::string>(value, property_name);
}

template<> StyleParser::Result<Function<TranslateAnchorType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<TranslateAnchorType>(value, property_name);
}

template<> StyleParser::Result<Function<RotateAnchorType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<RotateAnchorType>(value, property_name);
}

template<> StyleParser::Result<Function<CapType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<CapType>(value, property_name);
}

template<> StyleParser::Result<Function<JoinType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<JoinType>(value, property_name);
}

template<> StyleParser::Result<Function<PlacementType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<PlacementType>(value, property_name);
}

template<> StyleParser::Result<Function<TextAnchorType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<TextAnchorType>(value, property_name);
}

template<> StyleParser::Result<Function<TextJustifyType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<TextJustifyType>(value, property_name);
}

template<> StyleParser::Result<Function<TextTransformType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<TextTransformType>(value, property_name);
}

template<> StyleParser::Result<Function<RotationAlignmentType>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<RotationAlignmentType>(value, property_name);
}


template<> StyleParser::Result<Function<bool>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<bool>(value, property_name);
}

template<> StyleParser::Result<Function<float>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<float>(value, property_name);
}

template<> StyleParser::Result<Function<Color>> StyleParser::parseProperty(JSVal value, const char *property_name) {
    return parseFunction<Color>(value, property_name);
}

template<> StyleParser::Result<PiecewiseConstantFunction<Faded<std::vector<float>>>> StyleParser::parseProperty(JSVal value, const char *property_name, JSVal transition) {
    Duration duration = data.getDefaultFadeDuration();
    if (transition.HasMember("duration")) {
        duration = std::chrono::milliseconds(transition["duration"].GetUint());
    }

    if (value.IsObject()) {
        return parsePiecewiseConstantFunction<Faded<std::vector<float>>>(value, duration);
    } else if (value.IsArray()) {
        Faded<std::vector<float>> parsed;
        Result<std::vector<float>> floatarray = parseFloatArray(value);
        parsed.to = std::get<1>(floatarray);
        return Result<PiecewiseConstantFunction<Faded<std::vector<float>>>> { std::get<0>(floatarray),  { parsed, duration } };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be an array of numbers, or a number array function", property_name);
        return Result<PiecewiseConstantFunction<Faded<std::vector<float>>>> { StyleParserFailure, { {}, duration } };
    }
}

template<> StyleParser::Result<PiecewiseConstantFunction<Faded<std::string>>> StyleParser::parseProperty(JSVal value, const char *property_name, JSVal transition) {
    Duration duration = data.getDefaultFadeDuration();
    if (transition.HasMember("duration")) {
        duration = std::chrono::milliseconds(transition["duration"].GetUint());
    }

    if (value.IsObject()) {
        return parsePiecewiseConstantFunction<Faded<std::string>>(value, duration);
    } else if (value.IsString()) {
        Faded<std::string> parsed;
        parsed.to = { value.GetString(), value.GetStringLength() };
        return Result<PiecewiseConstantFunction<Faded<std::string>>> { StyleParserSuccess,  { parsed, duration } };
    } else {
        Log::Warning(Event::ParseStyle, "value of '%s' must be string or a string function", property_name);
        return Result<PiecewiseConstantFunction<Faded<std::string>>> { StyleParserFailure, { {}, duration } };
    }
}

#pragma mark - Parse Layers

void StyleParser::parseLayers(JSVal value) {
    if (!value.IsArray()) {
        Log::Warning(Event::ParseStyle, "layers must be an array");
        return;
    }

    for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
        JSVal layerValue = value[i];

        if (!layerValue.IsObject()) {
            Log::Warning(Event::ParseStyle, "layer must be an object");
            continue;
        }

        if (!layerValue.HasMember("id")) {
            Log::Warning(Event::ParseStyle, "layer must have an id");
            continue;
        }

        JSVal id = layerValue["id"];
        if (!id.IsString()) {
            Log::Warning(Event::ParseStyle, "layer id must be a string");
            continue;
        }

        const std::string layerID = { id.GetString(), id.GetStringLength() };
        if (layersMap.find(layerID) != layersMap.end()) {
            Log::Warning(Event::ParseStyle, "duplicate layer id %s", layerID.c_str());
            continue;
        }

        // Parse paints already, as they can't be inherited anyway.
        std::map<ClassID, ClassProperties> paints;
        parsePaints(layerValue, paints);

        util::ptr<StyleLayer> layer = std::make_shared<StyleLayer>(layerID, std::move(paints));

        layers.emplace_back(layer);
        layersMap.emplace(layerID, std::pair<JSVal, util::ptr<StyleLayer>> { layerValue, layer });
    }

    for (auto& pair : layersMap) {
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
    parseOptionalProperty<Function<std::array<float, 2>>>("fill-translate", Key::FillTranslate, klass, value);
    parseOptionalProperty<PropertyTransition>("fill-translate-transition", Key::FillTranslate, klass, value);
    parseOptionalProperty<Function<TranslateAnchorType>>("fill-translate-anchor", Key::FillTranslateAnchor, klass, value);
    parseOptionalProperty<PiecewiseConstantFunction<Faded<std::string>>>("fill-pattern", Key::FillImage, klass, value, "fill-pattern-transition");

    parseOptionalProperty<Function<float>>("line-opacity", Key::LineOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("line-opacity-transition", Key::LineOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("line-color", Key::LineColor, klass, value);
    parseOptionalProperty<PropertyTransition>("line-color-transition", Key::LineColor, klass, value);
    parseOptionalProperty<Function<std::array<float,2>>>("line-translate", Key::LineTranslate, klass, value);
    parseOptionalProperty<PropertyTransition>("line-translate-transition", Key::LineTranslate, klass, value);
    parseOptionalProperty<Function<TranslateAnchorType>>("line-translate-anchor", Key::LineTranslateAnchor, klass, value);
    parseOptionalProperty<Function<float>>("line-width", Key::LineWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("line-width-transition", Key::LineWidth, klass, value);
    parseOptionalProperty<Function<float>>("line-gap-width", Key::LineGapWidth, klass, value);
    parseOptionalProperty<PropertyTransition>("line-gap-width-transition", Key::LineGapWidth, klass, value);
    parseOptionalProperty<Function<float>>("line-blur", Key::LineBlur, klass, value);
    parseOptionalProperty<PropertyTransition>("line-blur-transition", Key::LineBlur, klass, value);
    parseOptionalProperty<PiecewiseConstantFunction<Faded<std::vector<float>>>>("line-dasharray", Key::LineDashArray, klass, value, "line-dasharray-transition");
    parseOptionalProperty<PiecewiseConstantFunction<Faded<std::string>>>("line-pattern", Key::LineImage, klass, value, "line-pattern-transition");

    parseOptionalProperty<Function<float>>("circle-radius", Key::CircleRadius, klass, value);
    parseOptionalProperty<Function<Color>>("circle-color", Key::CircleColor, klass, value);
    parseOptionalProperty<Function<float>>("circle-opacity", Key::CircleOpacity, klass, value);
    parseOptionalProperty<Function<std::array<float,2>>>("circle-translate", Key::CircleTranslate, klass, value);
    parseOptionalProperty<Function<TranslateAnchorType>>("circle-translate-anchor", Key::CircleTranslateAnchor, klass, value);
    parseOptionalProperty<Function<float>>("circle-blur", Key::CircleBlur, klass, value);

    parseOptionalProperty<Function<float>>("icon-opacity", Key::IconOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-opacity-transition", Key::IconOpacity, klass, value);
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
    parseOptionalProperty<Function<std::array<float, 2>>>("icon-translate", Key::IconTranslate, klass, value);
    parseOptionalProperty<PropertyTransition>("icon-translate-transition", Key::IconTranslate, klass, value);
    parseOptionalProperty<Function<TranslateAnchorType>>("icon-translate-anchor", Key::IconTranslateAnchor, klass, value);

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
    parseOptionalProperty<Function<std::array<float, 2>>>("text-translate", Key::TextTranslate, klass, value);
    parseOptionalProperty<PropertyTransition>("text-translate-transition", Key::TextTranslate, klass, value);
    parseOptionalProperty<Function<TranslateAnchorType>>("text-translate-anchor", Key::TextTranslateAnchor, klass, value);

    parseOptionalProperty<Function<float>>("raster-opacity", Key::RasterOpacity, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-opacity-transition", Key::RasterOpacity, klass, value);
    parseOptionalProperty<Function<float>>("raster-hue-rotate", Key::RasterHueRotate, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-hue-rotate-transition", Key::RasterHueRotate, klass, value);
    parseOptionalProperty<Function<float>>("raster-brightness-min", Key::RasterBrightnessLow, klass, value);
    parseOptionalProperty<Function<float>>("raster-brightness-max", Key::RasterBrightnessHigh, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-brightness-transition", Key::RasterBrightness, klass, value);
    parseOptionalProperty<Function<float>>("raster-saturation", Key::RasterSaturation, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-saturation-transition", Key::RasterSaturation, klass, value);
    parseOptionalProperty<Function<float>>("raster-contrast", Key::RasterContrast, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-contrast-transition", Key::RasterContrast, klass, value);
    parseOptionalProperty<Function<float>>("raster-fade-duration", Key::RasterFade, klass, value);
    parseOptionalProperty<PropertyTransition>("raster-fade-duration-transition", Key::RasterFade, klass, value);

    parseOptionalProperty<Function<float>>("background-opacity", Key::BackgroundOpacity, klass, value);
    parseOptionalProperty<Function<Color>>("background-color", Key::BackgroundColor, klass, value);
    parseOptionalProperty<PiecewiseConstantFunction<Faded<std::string>>>("background-pattern", Key::BackgroundImage, klass, value, "background-pattern-transition");
}

void StyleParser::parseLayout(JSVal value, util::ptr<StyleBucket> &bucket) {
    using Key = PropertyKey;

    parseVisibility<VisibilityType>(*bucket, value);

    parseOptionalProperty<Function<CapType>>("line-cap", Key::LineCap, bucket->layout, value);
    parseOptionalProperty<Function<JoinType>>("line-join", Key::LineJoin, bucket->layout, value);
    parseOptionalProperty<Function<float>>("line-miter-limit", Key::LineMiterLimit, bucket->layout, value);
    parseOptionalProperty<Function<float>>("line-round-limit", Key::LineRoundLimit, bucket->layout, value);

    parseOptionalProperty<Function<PlacementType>>("symbol-placement", Key::SymbolPlacement, bucket->layout, value);
    parseOptionalProperty<Function<float>>("symbol-spacing", Key::SymbolSpacing, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("symbol-avoid-edges", Key::SymbolAvoidEdges, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("icon-allow-overlap", Key::IconAllowOverlap, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("icon-ignore-placement", Key::IconIgnorePlacement, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("icon-optional", Key::IconOptional, bucket->layout, value);
    parseOptionalProperty<Function<RotationAlignmentType>>("icon-rotation-alignment", Key::IconRotationAlignment, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-max-size", Key::IconMaxSize, bucket->layout, value);
    parseOptionalProperty<Function<std::string>>("icon-image", Key::IconImage, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-rotate", Key::IconRotate, bucket->layout, value);
    parseOptionalProperty<Function<float>>("icon-padding", Key::IconPadding, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("icon-keep-upright", Key::IconKeepUpright, bucket->layout, value);
    parseOptionalProperty<Function<std::array<float, 2>>>("icon-offset", Key::IconOffset, bucket->layout, value);
    parseOptionalProperty<Function<RotationAlignmentType>>("text-rotation-alignment", Key::TextRotationAlignment, bucket->layout, value);
    parseOptionalProperty<Function<std::string>>("text-field", Key::TextField, bucket->layout, value);
    parseOptionalProperty<Function<std::string>>("text-font", Key::TextFont, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-max-size", Key::TextMaxSize, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-max-width", Key::TextMaxWidth, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-line-height", Key::TextLineHeight, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-letter-spacing", Key::TextLetterSpacing, bucket->layout, value);
    parseOptionalProperty<Function<TextJustifyType>>("text-justify", Key::TextJustify, bucket->layout, value);
    parseOptionalProperty<Function<TextAnchorType>>("text-anchor", Key::TextAnchor, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-max-angle", Key::TextMaxAngle, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-rotate", Key::TextRotate, bucket->layout, value);
    parseOptionalProperty<Function<float>>("text-padding", Key::TextPadding, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("text-keep-upright", Key::TextKeepUpright, bucket->layout, value);
    parseOptionalProperty<Function<TextTransformType>>("text-transform", Key::TextTransform, bucket->layout, value);
    parseOptionalProperty<Function<std::array<float, 2>>>("text-offset", Key::TextOffset, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("text-allow-overlap", Key::TextAllowOverlap, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("text-ignore-placement", Key::TextIgnorePlacement, bucket->layout, value);
    parseOptionalProperty<Function<bool>>("text-optional", Key::TextOptional, bucket->layout, value);

}

void StyleParser::parseReference(JSVal value, util::ptr<StyleLayer> &layer) {
    if (!value.IsString()) {
        Log::Warning(Event::ParseStyle, "layer ref of '%s' must be a string", layer->id.c_str());
        return;
    }
    const std::string ref { value.GetString(), value.GetStringLength() };
    auto it = layersMap.find(ref);
    if (it == layersMap.end()) {
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
    util::ptr<StyleBucket> bucket = std::make_shared<StyleBucket>(layer->type);

    // We name the buckets according to the layer that defined it.
    bucket->name = layer->id;

    if (value.HasMember("source")) {
        JSVal value_source = replaceConstant(value["source"]);
        if (value_source.IsString()) {
            bucket->source = { value_source.GetString(), value_source.GetStringLength() };
            auto source_it = sourcesMap.find(bucket->source);
            if (source_it == sourcesMap.end()) {
                Log::Warning(Event::ParseStyle, "can't find source '%s' required for layer '%s'", bucket->source.c_str(), layer->id.c_str());
            }
        } else {
            Log::Warning(Event::ParseStyle, "source of layer '%s' must be a string", layer->id.c_str());
        }
    }

    if (value.HasMember("source-layer")) {
        JSVal value_source_layer = replaceConstant(value["source-layer"]);
        if (value_source_layer.IsString()) {
            bucket->source_layer = { value_source_layer.GetString(), value_source_layer.GetStringLength() };
        } else {
            Log::Warning(Event::ParseStyle, "source-layer of layer '%s' must be a string", layer->id.c_str());
        }
    }

    if (value.HasMember("filter")) {
        JSVal value_filter = replaceConstant(value["filter"]);
        bucket->filter = parseFilterExpression(value_filter);
    }

    if (value.HasMember("layout")) {
        JSVal value_render = replaceConstant(value["layout"]);
        parseLayout(value_render, bucket);
    }

    if (value.HasMember("minzoom")) {
        JSVal min_zoom = value["minzoom"];
        if (min_zoom.IsNumber()) {
            bucket->min_zoom = min_zoom.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "minzoom of layer %s must be numeric", layer->id.c_str());
        }
    }

    if (value.HasMember("maxzoom")) {
        JSVal max_zoom = value["maxzoom"];
        if (max_zoom.IsNumber()) {
            bucket->max_zoom = max_zoom.GetDouble();
        } else {
            Log::Warning(Event::ParseStyle, "maxzoom of layer %s must be numeric", layer->id.c_str());
        }
    }

    layer->bucket = bucket;
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
