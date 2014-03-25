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
            fill.enabled = layer.enabled(z);
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
            stroke.enabled = layer.enabled(z);
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
            point.enabled = layer.enabled(z);
            point.color = layer.color;
            point.size = layer.size(z);
            point.opacity = layer.opacity(z);
            point.image = layer.image;
        }

        for (const auto& text_pair : sheetClass.text) {
            const std::string& layer_name = text_pair.first;
            const llmr::TextClass& layer = text_pair.second;

            // TODO: This should be restricted to point styles that have actual
            // values so as to not override with default values.
            llmr::TextProperties& text = computed.texts[layer_name];
            text.enabled = layer.enabled(z);
            text.color = layer.color;
            text.size = layer.size(z);
            text.halo = layer.halo;
            text.haloRadius = layer.haloRadius(z);
            text.rotate = layer.rotate(z);
            text.alwaysVisible = layer.alwaysVisible(z);
        }

        // Cascade background
        computed.background.color = sheetClass.background.color;
    }
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
            parser.parseClasses(document["classes"], classes);
        }
    } else {
        throw Style::exception("stylesheet root must be an object");
    }
}

