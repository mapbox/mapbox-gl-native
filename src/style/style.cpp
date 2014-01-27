#include <llmr/style/style.hpp>

#include <llmr/resources/style.hpp>

using namespace llmr;

Style::Style() {
    appliedClasses.insert("default");
}

void Style::reset() {
    computed.fills.clear();
    computed.strokes.clear();
}

void Style::load(const uint8_t *const data, uint32_t bytes) {
    pbf style(data, bytes);

    while (style.next()) {
        if (style.tag == 1) { // bucket
            buckets.insert(loadBucket(style.message()));
        } else if (style.tag == 2) { // structure
            layers.push_back(loadLayer(style.message()));
        } else if (style.tag == 3) { // class
            classes.insert(loadClass(style.message()));
        } else {
            style.skip();
        }
    }

    cascade();
}

std::pair<std::string, BucketDescription> Style::loadBucket(pbf data) {
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
            bucket.cap = (CapType)data.varint();
        } else if (data.tag == 8) { // join
            bucket.join = (JoinType)data.varint();
        } else if (data.tag == 9) { // font
            bucket.font = data.string();
        } else if (data.tag == 10) { // font_size
            bucket.font_size = data.float32();
        } else {
            data.skip();
        }
    }

    return { name, bucket };
}

LayerDescription Style::loadLayer(pbf data) {
    LayerDescription layer;

    while (data.next()) {
        if (data.tag == 1) { // name
            layer.name = data.string();
        } else if (data.tag == 2) { // bucket_name
            layer.bucket_name = data.string();
        } else if (data.tag == 3) { // child_layer
            layer.child_layer.emplace_back(loadLayer(data.message()));
        } else {
            data.skip();
        }
    }

    return layer;
}


std::pair<std::string, ClassDescription> Style::loadClass(pbf data) {
    ClassDescription klass;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // layer_style
            klass.insert(loadLayerStyle(data.message()));
        } else {
            data.skip();
        }
    }

    return { name, klass };
}

std::pair<std::string, LayerStyleDescription> Style::loadLayerStyle(pbf data) {
    LayerStyleDescription layerStyle;
    std::string name;

    while (data.next()) {
        if (data.tag == 1) { // name
            name = data.string();
        } else if (data.tag == 2) { // color
            uint32_t rgba = data.fixed<uint32_t, 4>();
            layerStyle.color = {{
                (float)((rgba >> 24) & 0xFF) / 0xFF,
                (float)((rgba >> 16) & 0xFF) / 0xFF,
                (float)((rgba >>  8) & 0xFF) / 0xFF,
                (float)((rgba >>  0) & 0xFF) / 0xFF
            }};
        } else if (data.tag == 3) { // antialias
            layerStyle.antialias = data.boolean();
        } else if (data.tag == 4) { // width
            layerStyle.width = loadWidth(data.message());
        } else {
            data.skip();
        }
    }

    return { name, layerStyle };
}

WidthDescription Style::loadWidth(pbf data) {
    WidthDescription width;

    while (data.next()) {
        if (data.tag == 1) { // scaling
            width.scaling = data.string();
        } else if (data.tag == 2) { // value
            // read a packed float32
            pbf floats = data.message();
            while (floats) {
                width.value.push_back(floats.float32());
            }
        } else {
            data.skip();
        }
    }

    return width;
}


void Style::cascade() {
    reset();

    // Recalculate style
    // Basic cascading
    for (const auto& class_pair : classes) {
        const std::string& class_name = class_pair.first;
        const ClassDescription& sheetClass = class_pair.second;

        // Not enabled
        if (appliedClasses.find(class_name) == appliedClasses.end()) continue;

        for (const auto& layer_pair : sheetClass) {
            const std::string& layer_name = layer_pair.first;
            const LayerStyleDescription& layer = layer_pair.second;

            // Find out what type this layer style is.
            computed.fills[layer_name].enabled = true;
            computed.fills[layer_name].antialiasing = layer.antialias;
            computed.fills[layer_name].fill_color = layer.color;
            computed.fills[layer_name].stroke_color = layer.color;
        }
    }
}
