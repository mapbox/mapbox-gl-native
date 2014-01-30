#include <llmr/style/style.hpp>

using namespace llmr;

Style::Style() {
    appliedClasses.insert("default");
}

void Style::reset() {
    computed.fills.clear();
    computed.lines.clear();
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
        } else if (data.tag == 2) { // fill_style
            klass.fill.insert(parseFillClass(data.message()));
        } else if (data.tag == 3) { // stroke_style
            klass.line.insert(parseLineClass(data.message()));
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
    }
}
