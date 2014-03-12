#include <llmr/style/style.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>


using namespace llmr;

Style::Style() {
    appliedClasses.insert("default");
}

void Style::reset() {
    computed.fills.clear();
    computed.lines.clear();
    computed.points.clear();
}

void Style::cascade(float z) {
    reset();

    // Recalculate style
    // Basic cascading
    for (const auto& class_pair : classes) {
        const std::string& class_name = class_pair.first;
        const ClassDescription& sheetClass = class_pair.second;

        // Not enabled
        if (appliedClasses.find(class_name) == appliedClasses.end()) continue;

        // Cascade fill classes
        for (const auto& fill_pair : sheetClass.fill) {
            const std::string& layer_name = fill_pair.first;
            const llmr::FillClass& layer = fill_pair.second;

            // TODO: This should be restricted to fill styles that have actual
            // values so as to not override with default values.
            llmr::FillProperties& fill = computed.fills[layer_name];
            fill.hidden = layer.hidden(z);
            fill.winding = layer.winding;
            fill.antialias = layer.antialias(z);
            fill.fill_color = layer.fill_color;
            fill.stroke_color = layer.stroke_color;
            fill.opacity = layer.opacity(z);
            fill.image = layer.image;
        }

        // Cascade line classes
        for (const auto& line_pair : sheetClass.line) {
            const std::string& layer_name = line_pair.first;
            const llmr::LineClass& layer = line_pair.second;

            // TODO: This should be restricted to line styles that have actual
            // values so as to not override with default values.
            llmr::LineProperties& stroke = computed.lines[layer_name];
            stroke.hidden = layer.hidden(z);
            stroke.width = layer.width(z);
            stroke.offset = layer.offset(z);
            stroke.color = layer.color;
            stroke.opacity = layer.opacity(z);
        }

        // Cascade point classes
        for (const auto& point_pair : sheetClass.point) {
            const std::string& layer_name = point_pair.first;
            const llmr::PointClass& layer = point_pair.second;

            // TODO: This should be restricted to point styles that have actual
            // values so as to not override with default values.
            llmr::PointProperties& point = computed.points[layer_name];
            point.hidden = layer.hidden(z);
            point.color = layer.color;
            point.size = layer.size(z);
            point.opacity = layer.opacity(z);
            point.image = layer.image;
        }

        // Cascade background
        computed.background.color = sheetClass.background.color;
    }
}



void loadBucketsJSON(const rapidjson::Value& value, std::map<std::string, BucketDescription>& buckets);
BucketDescription loadBucketJSON(const rapidjson::Value& value);
void loadLayersJSON(const rapidjson::Value& value, std::vector<LayerDescription>& layers);
LayerDescription loadLayerJSON(const rapidjson::Value& value);
void loadClassesJSON(const rapidjson::Value& value, std::map<std::string, ClassDescription>& classes);
std::pair<std::string, ClassDescription> loadClassDescriptionJSON(const rapidjson::Value& value);
void loadClassJSON(const std::string& name, const rapidjson::Value& value, ClassDescription& class_desc);
FillClass loadFillClassJSON(const rapidjson::Value& value);
LineClass loadLineClassJSON(const rapidjson::Value& value);
PointClass loadPointClassJSON(const rapidjson::Value& value);
BackgroundClass loadBackgroundClassJSON(const rapidjson::Value& value);
Value loadValueJSON(const rapidjson::Value& value);

void Style::loadJSON(const uint8_t *const data, size_t bytes) {
    rapidjson::Document document;

    if (bytes <= 0) {
        return;
    }

    if (data[bytes - 1] != 0) {
        throw exception("style JSON string is not 0-terminated");
    }

    document.Parse<0>((const char *const)data);

    buckets.clear();
    layers.clear();
    classes.clear();

    if (document.IsObject()) {
        if (document.HasMember("buckets")) {
            loadBucketsJSON(document["buckets"], buckets);
        }

        if (document.HasMember("structure")) {
            loadLayersJSON(document["structure"], layers);
        }

        if (document.HasMember("classes")) {
            loadClassesJSON(document["classes"], classes);
        }
    } else {
        throw Style::exception("stylesheet root must be an object");
    }
}

void loadBucketsJSON(const rapidjson::Value& value, std::map<std::string, BucketDescription>& buckets) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            buckets.emplace(itr->name.GetString(), std::forward<BucketDescription>(loadBucketJSON(itr->value)));
        }
    } else {
        throw Style::exception("buckets must be an object");
    }
}

BucketDescription loadBucketJSON(const rapidjson::Value& value) {
    BucketDescription bucket;

    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const std::string name(itr->name.GetString(), itr->name.GetStringLength());
        const rapidjson::Value& value = itr->value;

        if (name == "type") {
            if (value.IsString()) {
                bucket.type = bucketType({ value.GetString(), value.GetStringLength() });
            } else {
                throw Style::exception("bucket type must be a string");
            }
        } else if (name == "datasource") {
            if (value.IsString()) {
                bucket.source_name = { value.GetString(), value.GetStringLength() };
            } else {
                throw Style::exception("datasource name must be a string");
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
                    bucket.source_value.push_back(loadValueJSON(value[i]));
                }
            } else {
                bucket.source_value.push_back(loadValueJSON(value));
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

void loadLayersJSON(const rapidjson::Value& value, std::vector<LayerDescription>& layers) {
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            layers.push_back(loadLayerJSON(value[i]));
        }
    } else {
        throw Style::exception("structure must be an array");
    }
}

LayerDescription loadLayerJSON(const rapidjson::Value& value) {
    LayerDescription layer;

    if (value.IsObject()) {
        if (value.HasMember("name")) {
            const rapidjson::Value& name = value["name"];
            if (name.IsString()) {
                layer.name = { name.GetString(), name.GetStringLength() };
            } else {
                throw Style::exception("structure element name must be a string");
            }
        } else {
            throw Style::exception("structure element must have a name");
        }

        if (value.HasMember("bucket")) {
            const rapidjson::Value& bucket = value["bucket"];
            if (bucket.IsString()) {
                layer.bucket_name = { bucket.GetString(), bucket.GetStringLength() };
            } else {
                throw Style::exception("structure element bucket must be a string");
            }
        } else if (value.HasMember("layers")) {
            loadLayersJSON(value["layers"], layer.child_layer);
        } else {
            throw Style::exception("structure element must have either a bucket name or child layers");
        }
    } else {
        throw Style::exception("structure element must be an object");
    }

    return layer;
}

void loadClassesJSON(const rapidjson::Value& value, std::map<std::string, ClassDescription>& classes) {
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            classes.insert(std::forward<std::pair<std::string, ClassDescription>>(loadClassDescriptionJSON(value[i])));
        }
    } else {
        throw Style::exception("classes must be an array");
    }
}

std::pair<std::string, ClassDescription> loadClassDescriptionJSON(const rapidjson::Value& value) {
    ClassDescription klass;
    std::string klass_name;

    if (value.IsObject()) {
        if (value.HasMember("name")) {
            const rapidjson::Value& name = value["name"];
            if (name.IsString()) {
                klass_name = { name.GetString(), name.GetStringLength() };
            } else {
                throw Style::exception("class name must be a string");
            }
        } else {
            throw Style::exception("class must have a name");
        }

        if (value.HasMember("layers")) {
            const rapidjson::Value& layers = value["layers"];

            if (layers.IsObject()) {
                rapidjson::Value::ConstMemberIterator itr = layers.MemberBegin();
                for (; itr != layers.MemberEnd(); ++itr) {
                    const std::string name {
                        itr->name.GetString(), itr->name.GetStringLength()
                    };
                    loadClassJSON(name, itr->value, klass);
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

void loadClassJSON(const std::string& name, const rapidjson::Value& value, ClassDescription& class_desc) {
    if (value.IsObject()) {
        if (value.HasMember("type")) {
            const rapidjson::Value& type = value["type"];
            if (type.IsString()) {
                std::string type_name = { type.GetString(), type.GetStringLength() };
                if (type_name == "fill") {
                    class_desc.fill.insert({ name, std::forward<FillClass>(loadFillClassJSON(value)) });
                } else if (type_name == "line") {
                    class_desc.line.insert({ name, std::forward<LineClass>(loadLineClassJSON(value)) });
                } else if (type_name == "point") {
                    class_desc.point.insert({ name, std::forward<PointClass>(loadPointClassJSON(value)) });
                } else if (type_name == "background") {
                    class_desc.background = loadBackgroundClassJSON(value);
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

bool parseBoolean(const rapidjson::Value& value) {
    if (!value.IsBool()) {
        throw Style::exception("boolean value must be a boolean");
    }

    return value.GetBool();
}

std::string parseString(const rapidjson::Value& value) {
    if (!value.IsString()) {
        throw Style::exception("string value must be a string");
    }

    return { value.GetString(), value.GetStringLength() };
}

Color parseColor(const rapidjson::Value& value) {
    if (!value.IsString()) {
        throw Style::exception("color value must be a string");
    }

    CSSColorParser::Color css_color = CSSColorParser::parse({
        value.GetString(),
        value.GetStringLength()
    });

    return {{
            (float)css_color.r / 255,
            (float)css_color.g / 255,
            (float)css_color.b / 255,
            css_color.a
        }
    };
}

template <typename T>
typename FunctionProperty<T>::fn parseFunctionType(const rapidjson::Value& type) {
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

FunctionProperty<float> parseFloatFunction(const rapidjson::Value& value) {
    FunctionProperty<float> property;

    if (value.IsArray()) {
        if (value.Size() < 1) {
            throw Style::exception("value function does not have arguments");
        }

        property.function = parseFunctionType<float>(value[(rapidjson::SizeType)0]);
        for (rapidjson::SizeType i = 1; i < value.Size(); ++i) {
            const rapidjson::Value& stop = value[i];
            if (stop.IsObject()) {
                if (!stop.HasMember("z")) {
                    throw Style::exception("stop must have zoom level specification");
                }
                const rapidjson::Value& z = stop["z"];
                if (!z.IsNumber()) {
                    throw Style::exception("zoom level in stops must be a number");
                }
                property.values.push_back(z.GetDouble());

                if (!stop.HasMember("val")) {
                    throw Style::exception("stop must have value specification");
                }
                const rapidjson::Value& val = stop["val"];
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

FunctionProperty<bool> parseBoolFunction(const rapidjson::Value& value) {
    FunctionProperty<bool> property;

    if (value.IsArray()) {
        if (value.Size() < 1) {
            throw Style::exception("value function does not have arguments");
        }

        property.function = parseFunctionType<bool>(value[(rapidjson::SizeType)0]);
        for (rapidjson::SizeType i = 1; i < value.Size(); ++i) {
            const rapidjson::Value& stop = value[i];
            if (stop.IsObject()) {
                if (!stop.HasMember("z")) {
                    throw Style::exception("stop must have zoom level specification");
                }
                const rapidjson::Value& z = stop["z"];
                if (!z.IsBool()) {
                    throw Style::exception("zoom level in stops must be a number");
                }
                property.values.push_back(z.GetBool());

                if (!stop.HasMember("val")) {
                    throw Style::exception("stop must have value specification");
                }
                const rapidjson::Value& val = stop["val"];
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

FillClass loadFillClassJSON(const rapidjson::Value& value) {
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

LineClass loadLineClassJSON(const rapidjson::Value& value) {
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

PointClass loadPointClassJSON(const rapidjson::Value& value) {
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

BackgroundClass loadBackgroundClassJSON(const rapidjson::Value& value) {
    BackgroundClass klass;

    if (value.HasMember("color")) {
        klass.color = parseColor(value["color"]);
    }

    return klass;
}

Value loadValueJSON(const rapidjson::Value& value) {
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
