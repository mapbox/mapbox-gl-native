#include <llmr/style/style_parser.hpp>
#include <llmr/style/properties.hpp>
#include <llmr/util/constants.hpp>
#include <csscolorparser/csscolorparser.hpp>

namespace llmr {

using JSVal = const rapidjson::Value&;

void StyleParser::parseBuckets(JSVal value, std::map<std::string, BucketDescription>& buckets) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            buckets.emplace(
                std::forward<std::string>({ itr->name.GetString(), itr->name.GetStringLength() }),
                std::forward<BucketDescription>(parseBucket(itr->value))
            );
        }
    } else {
        throw Style::exception("buckets must be an object");
    }
}

PropertyFilterExpression StyleParser::parseFilterOrExpression(JSVal value) {
    if (value.IsArray()) {
        // This is an expression.
        util::recursive_wrapper<PropertyExpression> expression;
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            JSVal filter_item = value[i];

            if (filter_item.IsString()) {
                expression.get().op = expressionOperatorType({ filter_item.GetString(), filter_item.GetStringLength() });
            } else {
                expression.get().operands.emplace_back(parseFilterOrExpression(filter_item));
            }
        }
        return std::move(expression);
    } else if (value.IsObject() && value.HasMember("field") && value.HasMember("value")) {
        // This is a filter.
        JSVal field = value["field"];
        JSVal val = value["value"];

        if (!field.IsString()) {
            throw Style::exception("field name must be a string");
        }

        const std::string field_name { field.GetString(), field.GetStringLength() };
        const FilterOperator op = [&]{
            if (value.HasMember("operator")) {
                JSVal op_val = value["operator"];
                return filterOperatorType({ op_val.GetString(), op_val.GetStringLength() });
            } else {
                return FilterOperator::Equal;
            }
        }();

        if (val.IsArray()) {
            // The filter has several values, so it's an OR sub-expression.
            util::recursive_wrapper<PropertyExpression> expression;
            for (rapidjson::SizeType i = 0; i < val.Size(); ++i) {
                expression.get().operands.emplace_back(util::recursive_wrapper<PropertyFilter>(PropertyFilter { field_name, op, parseValue(val[i]) }));
            }

            return std::move(expression);
        } else {
            // The filter only has a single value, so it is a real filter.
            return std::move(util::recursive_wrapper<PropertyFilter>(PropertyFilter { field_name, op, parseValue(val) }));
        }
    }

    return std::true_type();
}

BucketDescription StyleParser::parseBucket(JSVal value) {
    BucketDescription bucket;

    if (value.HasMember("type")) {
        JSVal type = value["type"];
        if (type.IsString()) {
            bucket.type = bucketType({ type.GetString(), type.GetStringLength() });
        } else {
            throw Style::exception("bucket type must be a string");
        }
    }

    if (value.HasMember("feature_type")) {
        JSVal feature_type = value["feature_type"];
        if (feature_type.IsString()) {
            bucket.feature_type = bucketType({ feature_type.GetString(), feature_type.GetStringLength() });
        } else {
            throw Style::exception("feature type must be a string");
        }
    }

    if (value.HasMember("source")) {
        JSVal source = value["source"];
        if (source.IsString()) {
            bucket.source_name = { source.GetString(), source.GetStringLength() };
        } else {
            throw Style::exception("source name must be a string");
        }
    }

    if (value.HasMember("layer")) {
        JSVal source_layer = value["layer"];
        if (source_layer.IsString()) {
            bucket.source_layer = { source_layer.GetString(), source_layer.GetStringLength() };
        } else {
            throw Style::exception("layer name must be a string");
        }
    }

    if (value.HasMember("filter")) {
        bucket.filter = std::move(parseFilterOrExpression(value["filter"]));
    } else {
        // Allow a filter triple to be specified at the bucket root as well.
        bucket.filter = std::move(parseFilterOrExpression(value));
    }

    if (bucket.feature_type == BucketType::None) {
        bucket.feature_type = bucket.type;
    }

    switch (bucket.type) {
    case BucketType::Fill: {
        bucket.render = BucketFillDescription{};
    } break;

    case BucketType::Line: {
        bucket.render = BucketLineDescription{};
        BucketLineDescription &render = bucket.render.get<BucketLineDescription>();

        if (value.HasMember("cap")) {
            JSVal property = value["cap"];
            if (property.IsString()) {
                render.cap = capType({property.GetString(), property.GetStringLength()});
            } else {
                throw Style::exception("cap type must be a string");
            }
        }
        if (value.HasMember("join")) {
            JSVal property = value["join"];
            if (property.IsString()) {
                render.join = joinType({property.GetString(), property.GetStringLength()});
            } else {
                throw Style::exception("join type must be a string");
            }
        }
        if (value.HasMember("miterLimit")) {
            JSVal property = value["miterLimit"];
            if (property.IsNumber()) {
                render.miter_limit = property.GetDouble();
            } else {
                throw Style::exception("miter limit must be a number");
            }
        }
        if (value.HasMember("roundLimit")) {
            JSVal property = value["roundLimit"];
            if (property.IsNumber()) {
                render.round_limit = property.GetDouble();
            } else {
                throw Style::exception("round limit must be a number");
            }
        }
    } break;

    case BucketType::Icon: {
        bucket.render = BucketIconDescription{};
        BucketIconDescription &render = bucket.render.get<BucketIconDescription>();

        if (value.HasMember("size")) {
            JSVal property = value["size"];
            if (property.IsNumber()) {
                render.size = property.GetDouble();
            } else {
                throw Style::exception("font size must be a number");
            }
        }

        if (value.HasMember("icon")) {
            JSVal property = value["icon"];
            if (property.IsString()) {
                render.icon = {property.GetString(), property.GetStringLength()};
            } else {
                throw Style::exception("text field must be a string");
            }
        }

        if (value.HasMember("translate")) {
            JSVal property = value["translate"];
            if (property.IsArray()) {
                render.translate.x = property[(rapidjson::SizeType)0].GetDouble() * 24;
                render.translate.y = property[(rapidjson::SizeType)1].GetDouble() * -24;
            } else {
                throw Style::exception("translate must be a string");
            }
        }
    } break;

    case BucketType::Text: {
        bucket.render = BucketTextDescription{};
        BucketTextDescription &render = bucket.render.get<BucketTextDescription>();

        if (value.HasMember("text_field")) {
            JSVal property = value["text_field"];
            if (property.IsString()) {
                render.field = {property.GetString(), property.GetStringLength()};
            } else {
                throw Style::exception("text field must be a string");
            }
        }

        if (value.HasMember("path")) {
            JSVal property = value["path"];
            if (property.IsString()) {
                render.path = textPathType({property.GetString(), property.GetStringLength()});
            } else {
                throw Style::exception("curve must be a string");
            }
        }

        if (value.HasMember("font")) {
            JSVal property = value["font"];
            if (property.IsString()) {
                render.font = {property.GetString(), property.GetStringLength()};
            } else {
                throw Style::exception("font stack must be a string");
            }
        }

        if (value.HasMember("fontSize")) {
            JSVal property = value["fontSize"];
            if (property.IsNumber()) {
                render.max_size = property.GetDouble();
            } else {
                throw Style::exception("font size must be a number");
            }
        }

        if (value.HasMember("maxWidth")) {
            JSVal property = value["maxWidth"];
            if (property.IsNumber()) {
                render.max_width = property.GetDouble() * 24;
            } else {
                throw Style::exception("max width must be a number");
            }
        }

        if (value.HasMember("lineHeight")) {
            JSVal property = value["lineHeight"];
            if (property.IsNumber()) {
                render.line_height = property.GetDouble() * 24;
            } else {
                throw Style::exception("line height must be a number");
            }
        }

        if (value.HasMember("letterSpacing")) {
            JSVal property = value["letterSpacing"];
            if (property.IsNumber()) {
                render.letter_spacing = property.GetDouble() * 24;
            } else {
                throw Style::exception("letter spacing must be a number");
            }
        }

        if (value.HasMember("alignment")) {
            JSVal property = value["alignment"];
            if (property.IsString()) {
                render.alignment =
                    alignmentType({property.GetString(), property.GetStringLength()});
            } else {
                throw Style::exception("alignment must be a string");
            }
        }

        if (value.HasMember("verticalAlignment")) {
            JSVal property = value["verticalAlignment"];
            if (property.IsString()) {
                render.vertical_alignment =
                    verticalAlignmentType({property.GetString(), property.GetStringLength()});
            } else {
                throw Style::exception("verticalAlignment must be a string");
            }
        }

        if (value.HasMember("translate")) {
            JSVal property = value["translate"];
            if (property.IsArray()) {
                render.translate.x = property[(rapidjson::SizeType)0].GetDouble() * 24;
                render.translate.y = property[(rapidjson::SizeType)1].GetDouble() * -24;
            } else {
                throw Style::exception("translate must be a string");
            }
        }

        if (value.HasMember("maxAngleDelta")) {
            JSVal property = value["maxAngleDelta"];
            if (property.IsNumber()) {
                render.max_angle_delta = property.GetDouble();
            } else {
                throw Style::exception("max angle delta must be a number");
            }
        }

        if (value.HasMember("textMinDistance")) {
            JSVal property = value["textMinDistance"];
            if (property.IsNumber()) {
                render.min_distance = property.GetDouble();
            } else {
                throw Style::exception("text min distance must be a number");
            }
        }
    } break;

    case BucketType::Raster: {
        bucket.render = BucketRasterDescription{};
    } break;

    default:
        break;
    }

    return bucket;
}

void StyleParser::parseLayers(JSVal value, std::vector<LayerDescription>& layers) {
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            layers.push_back(parseLayer(value[i]));
        }
    } else {
        throw Style::exception("structure must be an array");
    }
}

LayerDescription StyleParser::parseLayer(JSVal value) {
    LayerDescription layer;

    if (value.IsObject()) {
        if (value.HasMember("name")) {
            JSVal name = value["name"];
            if (name.IsString()) {
                layer.name = { name.GetString(), name.GetStringLength() };
            } else {
                throw Style::exception("structure element name must be a string");
            }
        } else {
            throw Style::exception("structure element must have a name");
        }

        if (value.HasMember("bucket")) {
            JSVal bucket = value["bucket"];
            if (bucket.IsString()) {
                layer.bucket_name = { bucket.GetString(), bucket.GetStringLength() };
            } else {
                throw Style::exception("structure element bucket must be a string");
            }
        } else if (value.HasMember("layers")) {
            parseLayers(value["layers"], layer.child_layer);
        } else {
            throw Style::exception("structure element must have either a bucket name or child layers");
        }
    } else {
        throw Style::exception("structure element must be an object");
    }

    return layer;
}

void StyleParser::parseConstants(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            constants.emplace(
                std::forward<std::string>({ itr->name.GetString(), itr->name.GetStringLength() }),
                &itr->value
            );
        }
    } else {
        throw Style::exception("constants must be an object");
    }
}

template<> TranslateAnchor StyleParser::parse(JSVal anchor) {
    if (anchor.IsString()) {
        std::string a { anchor.GetString(), anchor.GetStringLength() };
        if (a == "viewport") {
            return TranslateAnchor::Viewport;
        } else {
            return TranslateAnchor::Map;
        }
    } else {
        throw Style::exception("translate anchor must be a string");
    }
}


void collectLayerBuckets(std::map<std::string, std::string> &layerBuckets,
                         const std::vector<LayerDescription> &layers) {
    for (const auto& layer : layers) {
        layerBuckets.emplace(layer.name, layer.bucket_name);
        if (layer.child_layer.size()) {
            collectLayerBuckets(layerBuckets, layer.child_layer);
        }
    }
}

void StyleParser::parseClasses(JSVal value,
                               std::map<std::string, ClassDescription> &classes,
                               const std::map<std::string, BucketDescription> &buckets,
                               const std::vector<LayerDescription> &layers) {

    // Build a non-recursive mapping of layer => bucket.
    std::map<std::string, std::string> layerBuckets;
    collectLayerBuckets(layerBuckets, layers);

    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            classes.emplace(parseClassDescription(value[i], buckets, layerBuckets));
        }
    } else {
        throw Style::exception("classes must be an array");
    }
}

std::pair<std::string, ClassDescription>
StyleParser::parseClassDescription(JSVal value,
                                   const std::map<std::string, BucketDescription> &buckets,
                                   const std::map<std::string, std::string> &layerBuckets) {
    ClassDescription klass;
    std::string klass_name;

    if (value.IsObject()) {
        if (value.HasMember("name")) {
            JSVal name = value["name"];
            if (name.IsString()) {
                klass_name = { name.GetString(), name.GetStringLength() };
            } else {
                throw Style::exception("class name must be a string");
            }
        } else {
            throw Style::exception("class must have a name");
        }

        if (value.HasMember("layers")) {
            JSVal layers = value["layers"];

            if (layers.IsObject()) {
                rapidjson::Value::ConstMemberIterator itr = layers.MemberBegin();
                for (; itr != layers.MemberEnd(); ++itr) {
                    const std::string name {
                        itr->name.GetString(), itr->name.GetStringLength()
                    };
                    parseClass(name, itr->value, klass, buckets, layerBuckets);
                }
            } else {
                throw Style::exception("class layer styles must be an object");
            }
        } else {
            throw Style::exception("class must have layer styles");
        }
    }

    return { klass_name, std::forward<ClassDescription>(klass) };
}

void StyleParser::parseClass(const std::string &name, JSVal value, ClassDescription &class_desc,
                             const std::map<std::string, BucketDescription> &buckets,
                             const std::map<std::string, std::string> &layerBuckets) {
    if (!value.IsObject()) {
        throw Style::exception("style class must be an object");
    }

    auto layer_bucket_it = layerBuckets.find(name);
    if (layer_bucket_it == layerBuckets.end()) {
        if (debug::styleParseWarnings) {
            fprintf(stderr, "[WARNING] there is no layer associated with '%s'\n", name.c_str());
        }
        return;
    }

    const std::string &bucket_name = layer_bucket_it->second;
    if (bucket_name == "background") {
        // background buckets are fake
        class_desc.addClass("background", parseBackgroundClass(value));
    } else if (bucket_name.length() == 0) {
        // no bucket name == composite bucket.
        class_desc.addClass(name, parseCompositeClass(value));
    } else {
        auto bucket_it = buckets.find(bucket_name);
        if (bucket_it == buckets.end()) {
            if (debug::styleParseWarnings) {
                fprintf(stderr, "[WARNING] there is no bucket named '%s'\n", bucket_name.c_str());
            }
            return;
        }

        BucketType type = bucket_it->second.type;
        if (type == BucketType::Fill) {
            class_desc.addClass(name, parseFillClass(value));
        } else if (type == BucketType::Line) {
            class_desc.addClass(name, parseLineClass(value));
        } else if (type == BucketType::Icon) {
            class_desc.addClass(name, parseIconClass(value));
        } else if (type == BucketType::Text) {
            class_desc.addClass(name, parseTextClass(value));
        } else if (type == BucketType::Raster) {
            class_desc.addClass(name, parseRasterClass(value));
        } else {
            throw Style::exception("unknown class type name");
        }
    }
}

template<> bool StyleParser::parse(JSVal value) {
    if (!value.IsBool()) {
        throw Style::exception("boolean value must be a boolean");
    }

    return value.GetBool();
}

template<> std::string StyleParser::parse(JSVal value) {
    if (!value.IsString()) {
        throw Style::exception("string value must be a string");
    }

    return { value.GetString(), value.GetStringLength() };
}

JSVal StyleParser::replaceConstant(JSVal value) {
    if (value.IsString()) {
        const std::string string_value { value.GetString(), value.GetStringLength() };
        auto it = constants.find(string_value);
        if (it != constants.end()) {
            return *it->second;
        }
    }

    return value;
}

template<> Color StyleParser::parse(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsArray()) {
        // [ r, g, b, a] array
        if (rvalue.Size() != 4) {
            throw Style::exception("color array must have four elements");
        }

        JSVal r = rvalue[(rapidjson::SizeType)0], g = rvalue[1], b = rvalue[2], a = rvalue[3];
        if (!r.IsNumber() || !g.IsNumber() || !b.IsNumber() || !a.IsNumber()) {
            throw Style::exception("color values must be numbers");
        }

        // Premultiply the color.
        const double alpha = a.GetDouble();
        return {{static_cast<float>(alpha * r.GetDouble()),
                 static_cast<float>(alpha * g.GetDouble()),
                 static_cast<float>(alpha * b.GetDouble()),
                 static_cast<float>(alpha)}};

    } else if (!rvalue.IsString()) {
        throw Style::exception("color value must be a string");
    }

    CSSColorParser::Color css_color = CSSColorParser::parse({ rvalue.GetString(), rvalue.GetStringLength() });

    // Premultiply the color.
    const float factor = css_color.a / 255;
    return {{(float)css_color.r * factor,
             (float)css_color.g * factor,
             (float)css_color.b * factor,
             css_color.a}};
}

FunctionProperty::fn parseFunctionType(JSVal type) {
    if (type.IsString()) {
        std::string t { type.GetString(), type.GetStringLength() };
        if (t == "constant") {
            return &functions::constant;
        } else if (t == "linear") {
            return &functions::linear;
        } else if (t == "stops") {
            return &functions::stops;
        } else if (t == "exponential") {
            return &functions::exponential;
        } else if (t == "min") {
            return &functions::min;
        } else if (t == "max") {
            return &functions::max;
        } else {
            throw Style::exception("unknown function type");
        }
    } else {
        throw Style::exception("function type must be a string");
    }
}

template<> FunctionProperty StyleParser::parse(JSVal value) {
    JSVal rvalue = replaceConstant(value);

    FunctionProperty property;

    if (rvalue.IsArray()) {
        if (rvalue.Size() < 1) {
            throw Style::exception("value function does not have arguments");
        }

        property.function = parseFunctionType(rvalue[(rapidjson::SizeType)0]);
        for (rapidjson::SizeType i = 1; i < rvalue.Size(); ++i) {
            JSVal stop = rvalue[i];
            if (stop.IsObject()) {
                if (!stop.HasMember("z")) {
                    throw Style::exception("stop must have zoom level specification");
                }
                JSVal z = stop["z"];
                if (!z.IsNumber()) {
                    throw Style::exception("zoom level in stops must be a number");
                }
                property.values.push_back(z.GetDouble());

                if (!stop.HasMember("val")) {
                    throw Style::exception("stop must have value specification");
                }
                JSVal val = stop["val"];
                if (!val.IsNumber()) {
                    throw Style::exception("value in stops must be a number");
                }
                property.values.push_back(val.GetDouble());
            } else if (stop.IsNumber()) {
                property.values.push_back(stop.GetDouble());
            } else if (stop.IsBool()) {
                property.values.push_back(stop.GetBool());
            } else {
                throw Style::exception("function argument must be a numeric value");
            }
        }
    } else if (rvalue.IsNumber()) {
        property.function = &functions::constant;
        property.values.push_back(rvalue.GetDouble());
    }

    return property;
}

void StyleParser::parseFunctionArray(const char *property_name, const std::vector<ClassPropertyKey> &keys, ClassProperties &klass, JSVal value) {
    if (value.HasMember(property_name)) {
        JSVal rvalue = replaceConstant(value[property_name]);
        if (!rvalue.IsArray()) {
            throw Style::exception("array value must be an array");
        }


        if (rvalue.Size() != keys.size()) {
            throw Style::exception("array value has unexpected number of elements");
        }

        for (uint16_t i = 0; i < keys.size(); i++) {
            klass.emplace(keys[i], parse<FunctionProperty>(rvalue[(rapidjson::SizeType)i]));
        }
    }
}

void StyleParser::parseTransition(const char *property_name, ClassPropertyKey key, ClassProperties &klass, JSVal value) {
    uint16_t duration = 0, delay = 0;
    if (value.HasMember(property_name)) {
        JSVal elements = value[property_name];
        if (elements.IsObject()) {
            if (elements.HasMember("duration") && elements["duration"].IsNumber()) {
                duration = elements["duration"].GetUint();
            }
            if (elements.HasMember("delay") && elements["delay"].IsNumber()) {
                delay = elements["delay"].GetUint();
            }
        }
    }

    if (duration || delay) {
        klass.emplace(key, PropertyTransition { duration, delay });
    }
}

void StyleParser::parseGenericClass(ClassProperties &klass, JSVal value) {
    using Key = ClassPropertyKey;
    parse<FunctionProperty>("enabled", Key::Enabled, klass, value);
    parseFunctionArray("translate", { Key::TranslateX, Key::TranslateY }, klass, value);
    parseTransition("transition-translate", Key::TranslateTransition, klass, value);
    parse<TranslateAnchor>("translate-anchor", Key::TranslateAnchor, klass, value);
    parse<FunctionProperty>("opacity", Key::Opacity, klass, value);
    parseTransition("transition-opacity", Key::OpacityTransition, klass, value);
    parse<FunctionProperty>("prerender", Key::Prerender, klass, value);
    parse<FunctionProperty>("prerender-buffer", Key::PrerenderBuffer, klass, value);
    parse<FunctionProperty>("prerender-size", Key::PrerenderSize, klass, value);
    parse<FunctionProperty>("prerender-blur", Key::PrerenderBlur, klass, value);
}

ClassProperties StyleParser::parseFillClass(JSVal value) {
    ClassProperties klass(RenderType::Fill);
    parseGenericClass(klass, value);

    using Key = ClassPropertyKey;
    parse<Color>("color", Key::FillColor, klass, value);
    parseTransition("transition-color", Key::FillColorTransition, klass, value);
    parse<Color>("stroke", Key::FillStrokeColor, klass, value);
    parseTransition("transition-stroke", Key::FillStrokeColorTransition, klass, value);
    parse<bool>("antialias", Key::FillAntialias, klass, value);
    parse<std::string>("image", Key::FillImage, klass, value);

    return klass;
}

ClassProperties StyleParser::parseLineClass(JSVal value) {
    ClassProperties klass(RenderType::Line);
    parseGenericClass(klass, value);

    using Key = ClassPropertyKey;
    parse<Color>("color", Key::LineColor, klass, value);
    parseTransition("transition-color", Key::LineColorTransition, klass, value);
    parse<FunctionProperty>("width", Key::LineWidth, klass, value);
    parseTransition("transition-width", Key::LineWidthTransition, klass, value);
    parseFunctionArray("dasharray", { Key::LineDashLand, Key::LineDashGap }, klass, value);
    parseTransition("transition-dasharray", Key::LineDashTransition, klass, value);

    return klass;
}

ClassProperties StyleParser::parseIconClass(JSVal value) {
    ClassProperties klass(RenderType::Icon);
    parseGenericClass(klass, value);

    using Key = ClassPropertyKey;
    parse<Color>("color", Key::IconColor, klass, value);
    parseTransition("transition-color", Key::IconColorTransition, klass, value);
    parse<std::string>("image", Key::IconImage, klass, value);
    parse<FunctionProperty>("size", Key::IconSize, klass, value);
    parse<FunctionProperty>("radius", Key::IconRadius, klass, value);
    parseTransition("transition-radius", Key::IconRadiusTransition, klass, value);
    parse<FunctionProperty>("blur", Key::IconBlur, klass, value);
    parseTransition("transition-blur", Key::IconBlurTransition, klass, value);

    return klass;
}

ClassProperties StyleParser::parseTextClass(JSVal value) {
    ClassProperties klass(RenderType::Text);
    parseGenericClass(klass, value);

    using Key = ClassPropertyKey;

    parse<Color>("color", Key::TextColor, klass, value);
    parseTransition("transition-color", Key::TextColorTransition, klass, value);
    parse<Color>("stroke", Key::TextHaloColor, klass, value);
    parseTransition("transition-stroke", Key::TextHaloColorTransition, klass, value);
    parse<FunctionProperty>("strokeWidth", Key::TextHaloRadius, klass, value);
    parseTransition("transition-strokeWidth", Key::TextHaloRadiusTransition, klass, value);
    parse<FunctionProperty>("strokeBlur", Key::TextHaloBlur, klass, value);
    parseTransition("transition-strokeBlur", Key::TextHaloBlurTransition, klass, value);
    parse<FunctionProperty>("size", Key::TextSize, klass, value);
    parse<FunctionProperty>("rotate", Key::TextRotate, klass, value);
    parse<FunctionProperty>("alwaysVisible", Key::TextAlwaysVisible, klass, value);

    return klass;
}

ClassProperties StyleParser::parseRasterClass(JSVal value) {
    ClassProperties klass(RenderType::Raster);
    parseGenericClass(klass, value);

    return klass;
}

ClassProperties StyleParser::parseCompositeClass(JSVal value) {
    ClassProperties klass(RenderType::Composite);
    parseGenericClass(klass, value);

    return klass;
}

ClassProperties StyleParser::parseBackgroundClass(JSVal value) {
    ClassProperties klass(RenderType::Background);
    parseGenericClass(klass, value);

    using Key = ClassPropertyKey;
    parse<Color>("color", Key::BackgroundColor, klass, value);
    parseTransition("transition-color", Key::BackgroundColorTransition, klass, value);

    return klass;
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

        case rapidjson::kObjectType:
        case rapidjson::kArrayType:
            throw Style::exception("value cannot be an object or array");
            return false;
    }
    throw Style::exception("unhandled value type in style");
    return false;
}

}
