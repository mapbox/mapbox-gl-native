#include <llmr/style/style.hpp>

// #include <rapidjson/document.h>


using namespace llmr;

Style::Style() {
    appliedClasses.insert("default");
}

void Style::reset() {
    computed.fills.clear();
    computed.lines.clear();
    computed.points.clear();
}

void Style::load(const uint8_t *const data, uint32_t bytes) {
    pbf style(data, bytes);

    while (style.next()) {
        if (style.tag == 1) { // bucket
            buckets.insert(parseBucket(style.message()));
        } else if (style.tag == 2) { // structure
            layers.push_back(parseLayer(style.message()));
        } else if (style.tag == 3) { // class
            classes.insert(parseClass(style.message()));
        } else {
            style.skip();
        }
    }
}

std::pair<std::string, BucketDescription> Style::parseBucket(pbf data) {
    BucketDescription bucket;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // type
            bucket.type = (BucketType)data.varint();
        } else if (data.tag == 3) { // source_name
            bucket.source_name = data.string();
        } else if (data.tag == 4) { // source_layer
            bucket.source_layer = data.string();
        } else if (data.tag == 5) { // source_field
            bucket.source_field = data.string();
        } else if (data.tag == 6) { // source_value
            bucket.source_value.emplace_back(parseValue(data.message()));
        } else if (data.tag == 7) { // cap
            bucket.geometry.cap = (CapType)data.varint();
        } else if (data.tag == 8) { // join
            bucket.geometry.join = (JoinType)data.varint();
        } else if (data.tag == 9) { // font
            bucket.geometry.font = data.string();
        } else if (data.tag == 10) { // font_size
            bucket.geometry.font_size = data.float32();
        } else {
            data.skip();
        }
    }

    return { name, bucket };
}

LayerDescription Style::parseLayer(pbf data) {
    LayerDescription layer;

    while (data.next()) {
        if (data.tag == 1) { // name
            layer.name = data.string();
        } else if (data.tag == 2) { // bucket_name
            layer.bucket_name = data.string();
        } else if (data.tag == 3) { // child_layer
            layer.child_layer.emplace_back(parseLayer(data.message()));
        } else {
            data.skip();
        }
    }

    return layer;
}


std::pair<std::string, ClassDescription> Style::parseClass(pbf data) {
    ClassDescription klass;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // fill style
            klass.fill.insert(parseFillClass(data.message()));
        } else if (data.tag == 3) { // line style
            klass.line.insert(parseLineClass(data.message()));
        } else if (data.tag == 4) { // point style
            klass.point.insert(parsePointClass(data.message()));
        } else {
            data.skip();
        }
    }

    return { name, klass };
}

std::pair<std::string, FillClass> Style::parseFillClass(pbf data) {
    FillClass fill;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // hidden
            fill.hidden = parseProperty<bool>(data.message());
        } else if (data.tag == 3) { // winding
            fill.winding = (Winding)data.varint();
        } else if (data.tag == 4) { // antialias
            fill.antialias = parseProperty<bool>(data.message());
        } else if (data.tag == 5) { // fill_color
            fill.fill_color = parseColor(data);
            if (fill.stroke_color[3] == std::numeric_limits<float>::infinity()) {
                fill.stroke_color = fill.fill_color;
            }
        } else if (data.tag == 6) { // stroke_color
            fill.stroke_color = parseColor(data);
        } else if (data.tag == 7) { // opacity
            fill.opacity = parseProperty<float>(data.message());
        } else if (data.tag == 8) { // image
            fill.image = data.string();
        } else {
            data.skip();
        }
    }

    return { name, fill };
}


std::pair<std::string, LineClass> Style::parseLineClass(pbf data) {
    LineClass stroke;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // hidden
            stroke.hidden = parseProperty<bool>(data.message());
        } else if (data.tag == 3) { // color
            stroke.color = parseColor(data);
        } else if (data.tag == 4) { // width
            stroke.width = parseProperty<float>(data.message());
        } else if (data.tag == 5) { // offset
            stroke.offset = parseProperty<float>(data.message());
        } else if (data.tag == 6) { // opacity
            stroke.opacity = parseProperty<float>(data.message());
        } else {
            data.skip();
        }
    }

    return { name, stroke };
}

std::pair<std::string, PointClass> Style::parsePointClass(pbf data) {
    PointClass point;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // hidden
            point.hidden = parseProperty<bool>(data.message());
        } else if (data.tag == 3) { // color
            point.color = parseColor(data);
        } else if (data.tag == 4) { // size
            point.size = parseProperty<float>(data.message());
        } else if (data.tag == 6) { // opacity
            point.opacity = parseProperty<float>(data.message());
        } else if (data.tag == 8) { // image
            point.image = data.string();
        } else {
            data.skip();
        }
    }

    return { name, point };
}

Color Style::parseColor(pbf& data) {
    uint32_t rgba = data.fixed<uint32_t, 4>();
    return {{
            (float)((rgba >> 24) & 0xFF) / 0xFF,
            (float)((rgba >> 16) & 0xFF) / 0xFF,
            (float)((rgba >>  8) & 0xFF) / 0xFF,
            (float)((rgba >>  0) & 0xFF) / 0xFF
        }
    };
}

template <typename T> FunctionProperty<T> Style::parseProperty(pbf data) {
    FunctionProperty<T> property;

    while (data.next()) {
        if (data.tag == 1) { // function
            switch ((Property)data.varint()) {
                case Property::Null: property.function = &functions::null; break;
                case Property::Constant: property.function = &functions::constant; break;
                case Property::Stops: property.function = &functions::stops; break;
                case Property::Linear: property.function = &functions::linear; break;
                default: property.function = &functions::null; break;
            }
        } else if (data.tag == 2) { // value
            // read a packed float32
            pbf floats = data.message();
            while (floats) {
                property.values.push_back(floats.float32());
            }
        } else {
            data.skip();
        }
    }

    return property;
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
    }
}

/*

void loadBucketsJSON(const rapidjson::Value& value, std::map<std::string, BucketDescription>& buckets);
BucketDescription loadBucketJSON(const rapidjson::Value& value);
void loadLayersJSON(const rapidjson::Value& value, std::vector<LayerDescription>& layers);
LayerDescription loadLayerJSON(const rapidjson::Value& value);
void loadClassesJSON(const rapidjson::Value& value, std::map<std::string, ClassDescription>& classes);
std::pair<std::string, ClassDescription> loadClassDescriptionJSON(const rapidjson::Value& value);
void loadClassJSON(const std::string& name, const rapidjson::Value& value, ClassDescription& class_desc);
FillClass loadFillClassJSON(const rapidjson::Value& value);
LineClass loadLineClassJSON(const rapidjson::Value& value);
Value loadValueJSON(const rapidjson::Value& value);

void Style::loadJSON(const std::string& data) {
    rapidjson::Document document;
    document.Parse<0>(data.c_str());

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
        fprintf(stderr, "stylesheet root must be an object\n");
    }
}

void loadBucketsJSON(const rapidjson::Value& value, std::map<std::string, BucketDescription>& buckets) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            buckets.emplace(itr->name.GetString(), loadBucketJSON(itr->value));
        }
    } else {
        fprintf(stderr, "buckets must be an object\n");
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
                fprintf(stderr, "bucket type must be a string\n");
            }
        } else if (name == "datasource") {
            if (value.IsString()) {
                bucket.source_name = { value.GetString(), value.GetStringLength() };
            } else {
                fprintf(stderr, "datasource name must be a string\n");
            }
        } else if (name == "layer") {
            if (value.IsString()) {
                bucket.source_layer = { value.GetString(), value.GetStringLength() };
            } else {
                fprintf(stderr, "layer name must be a string\n");
            }
        } else if (name == "field") {
            if (value.IsString()) {
                bucket.source_field = { value.GetString(), value.GetStringLength() };
            } else {
                fprintf(stderr, "field name must be a string\n");
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
                fprintf(stderr, "cap type must be a string\n");
            }
        } else if (name == "join") {
            if (value.IsString()) {
                bucket.geometry.join = joinType({ value.GetString(), value.GetStringLength() });
            } else {
                fprintf(stderr, "join type must be a string\n");
            }
        } else if (name == "font") {
            if (value.IsString()) {
                bucket.geometry.font = { value.GetString(), value.GetStringLength() };
            } else {
                fprintf(stderr, "font stack must be a string\n");
            }
        } else if (name == "fontSize") {
            if (value.IsNumber()) {
                bucket.geometry.font_size = value.GetDouble();
            } else {
                fprintf(stderr, "font size must be a number\n");
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
        fprintf(stderr, "structure must be an array\n");
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
                fprintf(stderr, "structure element name must be a string\n");
            }
        } else {
            fprintf(stderr, "structure element must have a name\n");
        }

        if (value.HasMember("bucket")) {
            const rapidjson::Value& bucket = value["bucket"];
            if (bucket.IsString()) {
                layer.bucket_name = { bucket.GetString(), bucket.GetStringLength() };
            } else {
                fprintf(stderr, "structure element bucket must be a string\n");
            }
        } else if (value.HasMember("layers")) {
            loadLayersJSON(value["layers"], layer.child_layer);
        } else {
            fprintf(stderr, "structure element must have either a bucket name or child layers\n");
        }
    } else {
        fprintf(stderr, "structure element must be an object\n");
    }

    return layer;
}


void loadClassesJSON(const rapidjson::Value& value, std::map<std::string, ClassDescription>& classes) {
    if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            classes.insert(loadClassDescriptionJSON(value[i]));
        }
    } else {
        fprintf(stderr, "classes must be an array\n");
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
                fprintf(stderr, "class name must be a string\n");
            }
        } else {
            fprintf(stderr, "class must have a name\n");
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
                fprintf(stderr, "class layer styles must be an object\n");
            }
        } else {
            fprintf(stderr, "class must have layer styles\n");
        }
    }

    return { klass_name, klass };
}

void loadClassJSON(const std::string& name, const rapidjson::Value& value, ClassDescription& class_desc) {
    if (value.IsObject()) {
        if (value.HasMember("type")) {
            const rapidjson::Value& type = value["type"];
            if (type.IsString()) {
                std::string type_name = { type.GetString(), type.GetStringLength() };
                if (type_name == "fill") {
                    class_desc.fill.insert({ type_name, loadFillClassJSON(value) });
                } else if (type_name == "line") {
                    class_desc.line.insert({ type_name, loadLineClassJSON(value) });
                } else {
                    // TODO:
                }
            } else {
                fprintf(stderr, "style class type must be a string\n");
            }
        } else {
            fprintf(stderr, "style class must specify a type\n");
        }
    } else {
        fprintf(stderr, "style class must be an object\n");
    }
}

FillClass loadFillClassJSON(const rapidjson::Value& value) {
    FillClass klass;

    if (value.HasMember("hidden")) {
        const rapidjson::Value& hidden = value["hidden"];
        // TODO...

    }

    return klass;
}

LineClass loadLineClassJSON(const rapidjson::Value& value) {
    FillClass klass;
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
            fprintf(stderr, "value cannot be an object or array\n");
            return false;
    }
}

*/
