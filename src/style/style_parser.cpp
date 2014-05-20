#include <llmr/style/style_parser.hpp>
#include <csscolorparser/csscolorparser.hpp>

using namespace llmr;

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

BucketDescription StyleParser::parseBucket(JSVal value) {
    BucketDescription bucket;

    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const std::string name(itr->name.GetString(), itr->name.GetStringLength());
        JSVal value = itr->value;

        if (name == "type") {
            if (value.IsString()) {
                bucket.type = bucketType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("bucket type must be a string");
            }
        } else if (name == "feature_type") {
            if (value.IsString()) {
                bucket.feature_type = bucketType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("feature type must be a string");
            }
        } else if (name == "source") {
            if (value.IsString()) {
                bucket.source_name = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("source name must be a string");
            }
        } else if (name == "layer") {
            if (value.IsString()) {
                bucket.source_layer = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("layer name must be a string");
            }
        } else if (name == "field") {
            if (value.IsString()) {
                bucket.source_field = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("field name must be a string");
            }
        } else if (name == "value") {
            if (value.IsArray()) {
                for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
                    bucket.source_value.push_back(parseValue(value[i]));
                }
            } else {
                bucket.source_value.push_back(parseValue(value));
            }
        } else if (name == "cap") {
            if (value.IsString()) {
                bucket.geometry.cap = capType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("cap type must be a string");
            }
        } else if (name == "join") {
            if (value.IsString()) {
                bucket.geometry.join = joinType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("join type must be a string");
            }
        } else if (name == "font") {
            if (value.IsString()) {
                bucket.geometry.font = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("font stack must be a string");
            }
        } else if (name == "fontSize" || name == "size") {
            if (value.IsNumber()) {
                bucket.geometry.size = value.GetDouble();
            } else {
                throw Style::exception("font size must be a number");
            }
        } else if (name == "text_field" || name == "icon") {
            if (value.IsString()) {
                bucket.geometry.field = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("text field must be a string");
            }
        } else if (name == "path") {
            if (value.IsString()) {
                bucket.geometry.path = textPathType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("curve must be a string");
            }
        } else if (name == "miterLimit") {
            if (value.IsNumber()) {
                bucket.geometry.miter_limit = value.GetDouble();
            } else {
                throw Style::exception("miter limit must be a number");
            }
        } else if (name == "roundLimit") {
            if (value.IsNumber()) {
                bucket.geometry.round_limit = value.GetDouble();
            } else {
                throw Style::exception("round limit must be a number");
            }
        } else if (name == "textMinDistance") {
            if (value.IsNumber()) {
                bucket.geometry.textMinDistance = value.GetDouble();
            } else {
                throw Style::exception("text min distance must be a number");
            }
        } else if (name == "maxAngleDelta") {
            if (value.IsNumber()) {
                bucket.geometry.maxAngleDelta = value.GetDouble();
            } else {
                throw Style::exception("max angle delta must be a number");
            }
        }

    }

    if (bucket.feature_type == BucketType::None) {
        bucket.feature_type = bucket.type;
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

TranslateAnchor parseTranslateAnchor(JSVal anchor) {
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
        fprintf(stderr, "there is no layer associated with style '%s'\n", name.c_str());
        return;
    }

    const std::string &bucket_name = layer_bucket_it->second;
    if (bucket_name == "background") {
        // background buckets are fake
        class_desc.background = parseBackgroundClass(value);
    } else {
        auto bucket_it = buckets.find(bucket_name);
        if (bucket_it == buckets.end()) {
            return;
        }

        BucketType type = bucket_it->second.type;
        if (type == BucketType::Fill) {
            class_desc.fill.insert({ name, std::forward<FillClass>(parseFillClass(value)) });
        } else if (type == BucketType::Line) {
            class_desc.line.insert({ name, std::forward<LineClass>(parseLineClass(value)) });
        } else if (type == BucketType::Icon) {
            class_desc.icon.insert({ name, std::forward<IconClass>(parseIconClass(value)) });
        } else if (type == BucketType::Text) {
            class_desc.text.insert({ name, std::forward<TextClass>(parseTextClass(value)) });
        } else if (type == BucketType::Raster) {
            class_desc.raster.insert({ name, std::forward<RasterClass>(parseRasterClass(value)) });
        } else {
            throw Style::exception("unknown class type name");
        }
    }
}

bool StyleParser::parseBoolean(JSVal value) {
    if (!value.IsBool()) {
        throw Style::exception("boolean value must be a boolean");
    }

    return value.GetBool();
}

std::string StyleParser::parseString(JSVal value) {
    if (!value.IsString()) {
        throw Style::exception("string value must be a string");
    }

    return { value.GetString(), value.GetStringLength() };
}

const JSVal& StyleParser::replaceConstant(const JSVal& value) {
    if (value.IsString()) {
        const std::string string_value { value.GetString(), value.GetStringLength() };
        auto it = constants.find(string_value);
        if (it != constants.end()) {
            return *it->second;
        }
    }

    return value;
}

std::vector<FunctionProperty> StyleParser::parseArray(JSVal value, uint16_t expected_count) {
    JSVal rvalue = replaceConstant(value);
    if (!rvalue.IsArray()) {
        throw Style::exception("array value must be an array");
    }

    if (rvalue.IsArray() && rvalue.Size() != expected_count) {
        throw Style::exception("array value has unexpected number of elements");
    }

    std::vector<FunctionProperty> values;
    values.reserve(expected_count);
    for (uint16_t i = 0; i < expected_count; i++) {
        values.push_back(parseFunction(rvalue[(rapidjson::SizeType)i]));
    }

    return values;
}

Color StyleParser::parseColor(JSVal value) {
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

FunctionProperty::fn StyleParser::parseFunctionType(JSVal type) {
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

FunctionProperty StyleParser::parseFunction(JSVal value) {
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

FillClass StyleParser::parseFillClass(JSVal value) {
    FillClass klass;

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("translate")) {
        std::vector<FunctionProperty> values = parseArray(value["translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("color")) {
        klass.fill_color = parseColor(value["color"]);
    }

    if (value.HasMember("stroke")) {
        klass.stroke_color = parseColor(value["stroke"]);
    } else {
        klass.stroke_color = klass.fill_color;
    }

    if (value.HasMember("antialias")) {
        klass.antialias = parseBoolean(value["antialias"]);
    }

    if (value.HasMember("image")) {
        klass.image = parseString(value["image"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

    return klass;
}

LineClass StyleParser::parseLineClass(JSVal value) {
    LineClass klass;

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("translate")) {
        std::vector<FunctionProperty> values = parseArray(value["translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    if (value.HasMember("width")) {
        klass.width = parseFunction(value["width"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

    if (value.HasMember("dasharray")) {
        std::vector<FunctionProperty> values = parseArray(value["dasharray"], 2);
        klass.dash_array = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    return klass;
}

IconClass StyleParser::parseIconClass(JSVal value) {
    IconClass klass;

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("translate")) {
        std::vector<FunctionProperty> values = parseArray(value["translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

    if (value.HasMember("image")) {
        klass.image = parseString(value["image"]);
    }

    if (value.HasMember("size")) {
        klass.size = parseFunction(value["size"]);
    }

    if (value.HasMember("radius")) {
        klass.radius = parseFunction(value["radius"]);
    }

    if (value.HasMember("blur")) {
        klass.blur = parseFunction(value["blur"]);
    }

    return klass;
}

TextClass StyleParser::parseTextClass(JSVal value) {
    TextClass klass;

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("translate")) {
        std::vector<FunctionProperty> values = parseArray(value["translate"], 2);
        klass.translate = std::array<FunctionProperty, 2> {{ values[0], values[1] }};
    }

    if (value.HasMember("translate-anchor")) {
        klass.translateAnchor = parseTranslateAnchor(value["translate-anchor"]);
    }

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    if (value.HasMember("stroke")) {
        klass.halo = parseColor(value["stroke"]);
    }

    if (value.HasMember("strokeWidth")) {
        klass.halo_radius = parseFunction(value["strokeWidth"]);
    }

    if (value.HasMember("strokeBlur")) {
        klass.halo_blur = parseFunction(value["strokeBlur"]);
    }

    if (value.HasMember("size")) {
        klass.size = parseFunction(value["size"]);
    }

    if (value.HasMember("rotate")) {
        klass.rotate = parseFunction(value["rotate"]);
    }

    if (value.HasMember("alwaysVisible")) {
        klass.alwaysVisible = parseFunction(value["alwaysVisible"]);
    }

    return klass;
}

RasterClass StyleParser::parseRasterClass(JSVal value) {
    RasterClass klass;

    if (value.HasMember("enabled")) {
        klass.enabled = parseFunction(value["enabled"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

    return klass;
}

BackgroundClass StyleParser::parseBackgroundClass(JSVal value) {
    BackgroundClass klass;

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

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
}
