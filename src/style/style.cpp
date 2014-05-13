#include <llmr/style/style.hpp>
#include <llmr/style/style_parser.hpp>
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
            fill.enabled = layer.enabled.evaluate<bool>(z);
            fill.translate = {{ layer.translate[0].evaluate<float>(z),
                                layer.translate[1].evaluate<float>(z) }};
            fill.winding = layer.winding;
            fill.antialias = layer.antialias.evaluate<bool>(z);
            fill.fill_color = layer.fill_color;
            fill.stroke_color = layer.stroke_color;
            fill.opacity = layer.opacity.evaluate<float>(z);
            fill.image = layer.image;
        }

        // Cascade line classes
        for (const auto& line_pair : sheetClass.line) {
            const std::string& layer_name = line_pair.first;
            const llmr::LineClass& layer = line_pair.second;

            // TODO: This should be restricted to line styles that have actual
            // values so as to not override with default values.
            llmr::LineProperties& stroke = computed.lines[layer_name];
            stroke.enabled = layer.enabled.evaluate<bool>(z);
            stroke.translate = {{ layer.translate[0].evaluate<float>(z),
                                  layer.translate[1].evaluate<float>(z) }};
            stroke.width = layer.width.evaluate<float>(z);
            stroke.offset = layer.offset.evaluate<float>(z);
            stroke.color = layer.color;
            stroke.dash_array = {{ layer.dash_array[0].evaluate<float>(z),
                                   layer.dash_array[1].evaluate<float>(z) }};
            stroke.opacity = layer.opacity.evaluate<float>(z);
        }

        // Cascade point classes
        for (const auto& point_pair : sheetClass.point) {
            const std::string& layer_name = point_pair.first;
            const llmr::PointClass& layer = point_pair.second;

            // TODO: This should be restricted to point styles that have actual
            // values so as to not override with default values.
            llmr::PointProperties& point = computed.points[layer_name];
            point.enabled = layer.enabled.evaluate<bool>(z);
            point.translate = {{ layer.translate[0].evaluate<float>(z),
                                 layer.translate[1].evaluate<float>(z) }};
            point.color = layer.color;
            point.size = layer.size.evaluate<float>(z);
            point.opacity = layer.opacity.evaluate<float>(z);
            point.image = layer.image;
            point.radius = layer.radius.evaluate<float>(z);
            point.blur = layer.blur.evaluate<float>(z);
        }

        // Cascade text classes
        for (const auto& text_pair : sheetClass.text) {
            const std::string& layer_name = text_pair.first;
            const llmr::TextClass& layer = text_pair.second;

            // TODO: This should be restricted to text styles that have actual
            // values so as to not override with default values.
            llmr::TextProperties& text = computed.texts[layer_name];
            text.enabled = layer.enabled.evaluate<bool>(z);
            text.translate = {{ layer.translate[0].evaluate<float>(z),
                                layer.translate[1].evaluate<float>(z) }};
            text.translateAnchor = layer.translateAnchor;
            text.color = layer.color;
            text.size = layer.size.evaluate<float>(z);
            text.halo = layer.halo;
            text.haloRadius = layer.haloRadius.evaluate<float>(z);
            text.rotate = layer.rotate.evaluate<float>(z);
            text.alwaysVisible = layer.alwaysVisible.evaluate<bool>(z);
            text.opacity = layer.opacity.evaluate<float>(z);
        }

        // Cascade raster classes
        for (const auto& raster_pair : sheetClass.raster) {
            const std::string& layer_name = raster_pair.first;
            const llmr::RasterClass& layer = raster_pair.second;

            // TODO: This should be restricted to raster styles that have actual
            // values so as to not override with default values.
            llmr::RasterProperties& raster = computed.rasters[layer_name];
            raster.enabled = layer.enabled.evaluate<bool>(z);
            raster.opacity = layer.opacity.evaluate<float>(z);
        }

        // Cascade background
        computed.background.color = sheetClass.background.color;
        computed.background.opacity = sheetClass.background.opacity.evaluate<float>(z);
    }
}

size_t Style::layerCount() const {
    size_t count = 0;
    for (const LayerDescription &layer : layers) {
        count += layer.size();
    }
    return count;
}

void Style::loadJSON(const uint8_t *const data, size_t bytes) {
    rapidjson::Document doc;

    if (bytes <= 0) {
        return;
    }

    if (data[bytes - 1] != 0) {
        throw exception("style JSON string is not 0-terminated");
    }

    doc.Parse<0>((const char *const)data);

    const rapidjson::Document& document = doc;

    buckets.clear();
    layers.clear();
    classes.clear();

    StyleParser parser;

    if (document.IsObject()) {
        if (document.HasMember("buckets")) {
            parser.parseBuckets(document["buckets"], buckets);
        }

        if (document.HasMember("structure")) {
            parser.parseLayers(document["structure"], layers);
        }

        if (document.HasMember("constants")) {
            parser.parseConstants(document["constants"]);
        }

        if (document.HasMember("classes")) {
            parser.parseClasses(document["classes"], classes, buckets, layers);
        }
    } else {
        throw Style::exception("stylesheet root must be an object");
    }
}

