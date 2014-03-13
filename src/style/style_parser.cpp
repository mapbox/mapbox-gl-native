#include <llmr/style/style_parser.hpp>
#include <csscolorparser/csscolorparser.hpp>

using namespace llmr;

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
        } else if (name == "fontSize") {
            if (value.IsNumber()) {
                bucket.geometry.font_size = value.GetDouble();
            } else {
                throw Style::exception("font size must be a number");
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
        }
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

void StyleParser::parseClasses(JSVal value, std::map<std::string, ClassDescription>& classes) {
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            classes.insert(std::forward<std::pair<std::string, ClassDescription>>(parseClassDescription(value[i])));
        }
    } else {
        throw Style::exception("classes must be an array");
    }
}

std::pair<std::string, ClassDescription> StyleParser::parseClassDescription(JSVal value) {
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
                    parseClass(name, itr->value, klass);
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

void StyleParser::parseClass(const std::string& name, JSVal value, ClassDescription& class_desc) {
    if (value.IsObject()) {
        if (value.HasMember("type")) {
            JSVal type = value["type"];
            if (type.IsString()) {
                std::string type_name = { type.GetString(), type.GetStringLength() };
                if (type_name == "fill") {
                    class_desc.fill.insert({ name, std::forward<FillClass>(parseFillClass(value)) });
                } else if (type_name == "line") {
                    class_desc.line.insert({ name, std::forward<LineClass>(parseLineClass(value)) });
                } else if (type_name == "point") {
                    class_desc.point.insert({ name, std::forward<PointClass>(parsePointClass(value)) });
                } else if (type_name == "background") {
                    class_desc.background = parseBackgroundClass(value);
                } else {
                    throw Style::exception("unkonwn class type name");
                }
            } else {
                throw Style::exception("style class type must be a string");
            }
        } else {
            throw Style::exception("style class must specify a type");
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

Color StyleParser::parseColor(JSVal value) {
    if (!value.IsString()) {
        throw Style::exception("color value must be a string");
    }

    const std::string str { value.GetString(), value.GetStringLength() };

    auto it = constants.find(str);
    if (it != constants.end()) {
        return parseColor(*it->second);
    } else {
        CSSColorParser::Color css_color = CSSColorParser::parse(str);
        return {{
                (float)css_color.r / 255,
                (float)css_color.g / 255,
                (float)css_color.b / 255,
                css_color.a
            }
        };
    }
}

template <typename T>
typename FunctionProperty<T>::fn StyleParser::parseFunctionType(JSVal type) {
    if (type.IsString()) {
        std::string t { type.GetString(), type.GetStringLength() };
        if (t == "constant") {
            return &functions::constant;
        } else if (t == "linear") {
            return &functions::linear;
        } else if (t == "stops") {
            return &functions::stops;
        } else {
            throw Style::exception("unknown function type");
        }
    } else {
        throw Style::exception("function type must be a string");
    }
}

FunctionProperty<float> StyleParser::parseFloatFunction(JSVal value) {
    FunctionProperty<float> property;

    if (value.IsArray()) {
        if (value.Size() < 1) {
            throw Style::exception("value function does not have arguments");
        }

        property.function = parseFunctionType<float>(value[(rapidjson::SizeType)0]);
        for (rapidjson::SizeType i = 1; i < value.Size(); ++i) {
            JSVal stop = value[i];
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
            } else {
                throw Style::exception("stop must be a number");
            }
        }
    } else if (value.IsNumber()) {
        property.function = &functions::constant;
        property.values.push_back(value.GetDouble());
    }

    return property;
}

FunctionProperty<bool> StyleParser::parseBoolFunction(JSVal value) {
    FunctionProperty<bool> property;

    if (value.IsArray()) {
        if (value.Size() < 1) {
            throw Style::exception("value function does not have arguments");
        }

        property.function = parseFunctionType<bool>(value[(rapidjson::SizeType)0]);
        for (rapidjson::SizeType i = 1; i < value.Size(); ++i) {
            JSVal stop = value[i];
            if (stop.IsObject()) {
                if (!stop.HasMember("z")) {
                    throw Style::exception("stop must have zoom level specification");
                }
                JSVal z = stop["z"];
                if (!z.IsBool()) {
                    throw Style::exception("zoom level in stops must be a number");
                }
                property.values.push_back(z.GetBool());

                if (!stop.HasMember("val")) {
                    throw Style::exception("stop must have value specification");
                }
                JSVal val = stop["val"];
                if (!val.IsBool()) {
                    throw Style::exception("value in stops must be a number");
                }
                property.values.push_back(val.GetBool());
            } else if (stop.IsBool()) {
                property.values.push_back(stop.GetBool());
            } else {
                throw Style::exception("stop must be a number");
            }
        }
    } else if (value.IsBool()) {
        property.function = &functions::constant;
        property.values.push_back(value.GetBool());
    }

    return property;
}

FillClass StyleParser::parseFillClass(JSVal value) {
    FillClass klass;

    if (value.HasMember("hidden")) {
        klass.hidden = parseBoolFunction(value["hidden"]);
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
        klass.opacity = parseFloatFunction(value["opacity"]);
    }

    return klass;
}

LineClass StyleParser::parseLineClass(JSVal value) {
    LineClass klass;

    if (value.HasMember("hidden")) {
        klass.hidden = parseBoolFunction(value["hidden"]);
    }

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    if (value.HasMember("width")) {
        klass.width = parseFloatFunction(value["width"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFloatFunction(value["opacity"]);
    }

    return klass;
}

PointClass StyleParser::parsePointClass(JSVal value) {
    PointClass klass;

    if (value.HasMember("hidden")) {
        klass.hidden = parseBoolFunction(value["hidden"]);
    }

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    if (value.HasMember("opacity")) {
        klass.opacity = parseFloatFunction(value["opacity"]);
    }

    if (value.HasMember("image")) {
        klass.image = parseString(value["image"]);
    }

    if (value.HasMember("size")) {
        klass.size = parseFloatFunction(value["size"]);
    }

    return klass;
}

BackgroundClass StyleParser::parseBackgroundClass(JSVal value) {
    BackgroundClass klass;

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
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
