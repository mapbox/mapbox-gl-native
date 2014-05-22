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
    bucket.type = BucketType::None;

    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const std::string name(itr->name.GetString(), itr->name.GetStringLength());
        JSVal value = itr->value;

        if (name == "text") {
            bucket.type = BucketType::Text;

        } else if (name == "point") {
            bucket.type = BucketType::Icon;

        } else if (name == "line") {
            bucket.type = BucketType::Line;

        } else if (name == "fill") {
            bucket.type = BucketType::Fill;

        } else if (name == "filter") {
            if (value.IsObject()) {
                rapidjson::Value::ConstMemberIterator itr2 = value.MemberBegin();
                for (; itr2 != value.MemberEnd(); ++itr2) {
                    const std::string name2(itr2->name.GetString(), itr2->name.GetStringLength());
                    JSVal value2 = itr2->value;

                    if (name2 == "feature_type") {
                        if (value2.IsString()) {
                            bucket.feature_type = bucketType({ value2.GetString(), value2.GetStringLength() });
                        } else {
                            throw Style::exception("feature type must be a string");
                        }

                    } else if (name2 == "source") {
                        if (value2.IsString()) {
                            bucket.source_name = { value2.GetString(), value2.GetStringLength() };
                        } else {
                            throw Style::exception("source name must be a string");
                        }

                    } else if (name2 == "layer") {
                        if (value2.IsString()) {
                            bucket.source_layer = { value2.GetString(), value2.GetStringLength() };
                        } else {
                            throw Style::exception("layer name must be a string");
                        }

                    } else {
                        // This catches "type", "class", "maki", "index", etc
                        if (value2.IsString() || value2.IsNumber() || value2.IsBool()) {
                            bucket.source_field = name2;
                            bucket.source_value.push_back(parseValue(value2));
                        } else if (value2.IsArray()) {
                            rapidjson::Value::ConstValueIterator itr3 = value2.Begin();
                            for (; itr3 != value2.End(); ++itr3) {
                                if (itr3->IsString() || itr3->IsNumber() || itr3->IsBool()) {
                                    bucket.source_field = name2;
                                    bucket.source_value.push_back(parseValue(itr3));
                                } else {
                                    printf("filter %s makes me confused\n", name2.c_str());
                                    throw Style::exception("Unexpected type for a filter array element value");
                                }
                            }
                        } else {
                            printf("filter %s makes me confused\n", name2.c_str());
                            throw Style::exception("Unexpected type for a filter value");
                        }
                    }
                }
            } else {
                throw Style::exception("filter type must be a object");
            }

        } else if (name == "line-cap") {
            if (value.IsString()) {
                bucket.geometry.cap = capType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("line-cap type must be a string");
            }
        } else if (name == "line-join") {
            if (value.IsString()) {
                bucket.geometry.join = joinType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("line-join type must be a string");
            }
        } else if (name == "font") {
            if (value.IsString()) {
                bucket.geometry.font = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("font stack must be a string");
            }
        } else if (name == "text-max-size" || name == "size") {
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
        } else if (name == "text-path") {
            if (value.IsString()) {
                bucket.geometry.path = textPathType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("text-path must be a string");
            }
        } else if (name == "line-miter-limit") {
            if (value.IsNumber()) {
                bucket.geometry.miter_limit = value.GetDouble();
            } else {
                throw Style::exception("line miter limit must be a number");
            }
        } else if (name == "line-round-limit") {
            if (value.IsNumber()) {
                bucket.geometry.round_limit = value.GetDouble();
            } else {
                throw Style::exception("line round limit must be a number");
            }
        } else if (name == "text-min-distance") {
            if (value.IsNumber()) {
                bucket.geometry.textMinDistance = value.GetDouble();
            } else {
                throw Style::exception("text min distance must be a number");
            }
        } else if (name == "text-max-angle") {
            if (value.IsNumber()) {
                bucket.geometry.maxAngleDelta = value.GetDouble();
            } else {
                throw Style::exception("text max angle delta must be a number");
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
        throw Style::exception("layers must be an array");
    }
}

LayerDescription StyleParser::parseLayer(JSVal value) {
    LayerDescription layer;

    if (value.IsObject()) {
        bool layerIsBackground = false;
        if (value.HasMember("id")) {
            JSVal name = value["id"];
            if (name.IsString()) {
                layer.name = { name.GetString(), name.GetStringLength() };
                if(layer.name == "background") {
                    layerIsBackground = true;
                }
            } else {
                throw Style::exception("layers element id must be a string");
            }
        } else {
            throw Style::exception("layers element must have an id");
        }

        if (value.HasMember("bucket")) {
            JSVal bucket = value["bucket"];
            if (bucket.IsString()) {
                layer.bucket_name = { bucket.GetString(), bucket.GetStringLength() };
            } else {
                throw Style::exception("layers element bucket must be a string");
            }
        } else if (value.HasMember("layers")) {
            parseLayers(value["layers"], layer.child_layer);
        } else if (layerIsBackground) {
            layer.bucket_name = "background";
        } else {
            throw Style::exception("layers element must be the background, have a bucket name, or have child layers");
        }
    } else {
        throw Style::exception("layer element must be an object");
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

void StyleParser::parseClasses(JSVal value, std::map<std::string, ClassDescription>& classes, std::map<std::string, BucketDescription>& buckets, std::vector<LayerDescription>& layers) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        std::string styleName;
        for (; itr != value.MemberEnd(); ++itr) {
            styleName = { itr->name.GetString(), itr->name.GetStringLength() };
            classes.insert(std::forward<std::pair<std::string, ClassDescription>>(parseClassDescription(styleName, itr->value, buckets, layers)));
        }
    } else {
        throw Style::exception("styles must be an object");
    }
}

std::pair<std::string, ClassDescription> StyleParser::parseClassDescription(std::string styleName, JSVal value, std::map<std::string, BucketDescription>& buckets, std::vector<LayerDescription>& style_layers) {
    ClassDescription klass;

    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            const std::string name {
                itr->name.GetString(), itr->name.GetStringLength()
            };
            parseClass(name, itr->value, klass, buckets, style_layers);
        }
    }

    return { styleName, std::forward<ClassDescription>(klass) };
}

void StyleParser::parseClass(const std::string& name, JSVal value, ClassDescription& class_desc, std::map<std::string, BucketDescription>& buckets, std::vector<LayerDescription>& layers) {
    if (value.IsObject()) {
        for (const auto& layer : layers) {
            if (name == layer.name) {
                if (layer.bucket_name == "background") {
                    // background buckets are fake
                    class_desc.background = parseBackgroundClass(value);
                } else {
                    std::string bucket_name = layer.bucket_name;
                    auto bucket_it = buckets.find(bucket_name);
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
                        printf("WARNING: Unable to determine bucket type for style '%s'. It might be using nested layers (un-implemented).\n",name.c_str());
                        //throw Style::exception("unknown class type name");
                    }
                }
            }
        }
    } else {
        throw Style::exception("style class must be an object");
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

    if (value.HasMember("fill-color")) {
        klass.fill_color = parseColor(value["fill-color"]);
    }

    if (value.HasMember("stroke-color")) {
        klass.stroke_color = parseColor(value["stroke-color"]);
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

    if (value.HasMember("line-color")) {
        klass.color = parseColor(value["line-color"]);
    }

    if (value.HasMember("line-width")) {
        klass.width = parseFunction(value["line-width"]);
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

    if (value.HasMember("point-color")) {
        klass.color = parseColor(value["point-color"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFunction(value["opacity"]);
    }

    if (value.HasMember("point-image")) {
        klass.image = parseString(value["point-image"]);
    }

    if (value.HasMember("size")) {
        klass.size = parseFunction(value["size"]);
    }

    if (value.HasMember("point-radius")) {
        klass.radius = parseFunction(value["point-radius"]);
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

    if (value.HasMember("text-color")) {
        klass.color = parseColor(value["text-color"]);
    }

    if (value.HasMember("text-halo-color")) {
        klass.halo = parseColor(value["text-halo-color"]);
    }

    if (value.HasMember("strokeWidth")) {
        klass.halo_radius = parseFunction(value["strokeWidth"]);
    }

    if (value.HasMember("strokeBlur")) {
        klass.halo_blur = parseFunction(value["strokeBlur"]);
    }

    if (value.HasMember("text-size")) {
        klass.size = parseFunction(value["text-size"]);
    }

    if (value.HasMember("text-rotate")) {
        klass.rotate = parseFunction(value["text-rotate"]);
    }

    if (value.HasMember("text-always-visible")) {
        klass.alwaysVisible = parseFunction(value["text-always-visible"]);
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
