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
    computed.texts.clear();
    computed.rasters.clear();
}

void Style::cascade(float z) {
    time start = util::now();

    previous.fills = computed.fills;
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

            // enabled
            fill.enabled = layer.enabled.evaluate<bool>(z);

            // translate (transitionable)
            if (layer.translate_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Translate) &&
                (layer.translate[0].evaluate<float>(z) != previous.fills[layer_name].translate[0] ||
                 layer.translate[1].evaluate<float>(z) != previous.fills[layer_name].translate[1])) {

                transitioning.fills[layer_name].translate = {{ previous.fills[layer_name].translate[0],
                                                               previous.fills[layer_name].translate[1] }};

                std::vector<float> from, to, transitioning_ref;
                from.push_back(previous.fills[layer_name].translate[0]);
                from.push_back(previous.fills[layer_name].translate[1]);
                to.push_back(layer.translate[0].evaluate<float>(z));
                to.push_back(layer.translate[1].evaluate<float>(z));
                transitioning_ref.push_back(transitioning.fills[layer_name].translate[0]);
                transitioning_ref.push_back(transitioning.fills[layer_name].translate[1]);
                transitions[layer_name][PropertyKey::Translate] = std::make_shared<util::ease_transition<std::vector<float>>>(
                                                                      from,
                                                                      to,
                                                                      transitioning_ref,
                                                                      start,
                                                                      layer.translate_transition.duration * 1_millisecond
                                                                  );
            } else if (transitions[layer_name].count(PropertyKey::Translate)) {
                fill.translate = transitioning.fills[layer_name].translate;
            } else {
                fill.translate = {{ layer.translate[0].evaluate<float>(z), layer.translate[1].evaluate<float>(z) }};
            }

            // translate anchor
            fill.translateAnchor = layer.translateAnchor;

            // winding
            fill.winding = layer.winding;

            // antialias
            fill.antialias = layer.antialias.evaluate<bool>(z);

            // fill color (transitionable)
            if (layer.fill_color_transition.duration &&
                !transitions[layer_name].count(PropertyKey::FillColor) &&
                layer.fill_color != previous.fills[layer_name].fill_color) {

                transitioning.fills[layer_name].fill_color = previous.fills[layer_name].fill_color;

                transitions[layer_name][PropertyKey::FillColor] = std::make_shared<util::ease_transition<Color>>(
                                                                      previous.fills[layer_name].fill_color,
                                                                      layer.fill_color,
                                                                      transitioning.fills[layer_name].fill_color,
                                                                      start,
                                                                      layer.fill_color_transition.duration * 1_millisecond
                                                                  );
            } else if (transitions[layer_name].count(PropertyKey::FillColor)) {
                fill.fill_color = transitioning.fills[layer_name].fill_color;
            }
            else {
                fill.fill_color = layer.fill_color;
            }

            // stroke color (transitionable)
            if (layer.stroke_color_transition.duration &&
                !transitions[layer_name].count(PropertyKey::StrokeColor) &&
                layer.stroke_color != previous.fills[layer_name].stroke_color) {

                transitioning.fills[layer_name].stroke_color = previous.fills[layer_name].stroke_color;

                transitions[layer_name][PropertyKey::StrokeColor] = std::make_shared<util::ease_transition<Color>>(
                                                                        previous.fills[layer_name].stroke_color,
                                                                        layer.stroke_color,
                                                                        transitioning.fills[layer_name].stroke_color,
                                                                        start,
                                                                        layer.stroke_color_transition.duration * 1_millisecond
                                                                    );
            } else if (transitions[layer_name].count(PropertyKey::StrokeColor)) {
                fill.stroke_color = transitioning.fills[layer_name].stroke_color;
            }
            else {
                fill.stroke_color = layer.stroke_color;
            }

            // opacity (transitionable)
            if (layer.opacity_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Opacity) &&
                layer.opacity.evaluate<float>(z) != previous.fills[layer_name].opacity) {

                transitioning.fills[layer_name].opacity = previous.fills[layer_name].opacity;

                transitions[layer_name][PropertyKey::Opacity] = std::make_shared<util::ease_transition<float>>(
                                                                    previous.fills[layer_name].opacity,
                                                                    layer.opacity.evaluate<float>(z),
                                                                    transitioning.fills[layer_name].opacity,
                                                                    start,
                                                                    layer.opacity_transition.duration * 1_millisecond
                                                                );
            } else if (transitions[layer_name].count(PropertyKey::Opacity)) {
                fill.opacity = transitioning.fills[layer_name].opacity;
            } else {
                fill.opacity = layer.opacity.evaluate<float>(z);
            }

            // image
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
            stroke.translateAnchor = layer.translateAnchor;
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
            point.translateAnchor = layer.translateAnchor;
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
            text.halo_radius = layer.halo_radius.evaluate<float>(z);
            text.rotate = layer.rotate.evaluate<float>(z);
            text.always_visible = layer.always_visible.evaluate<bool>(z);
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

bool Style::needsTransition() const {
    uv::readlock lock(mtx);

    for (auto layer_it = transitions.begin(); layer_it != transitions.end(); layer_it++) {
        for (auto prop_it = layer_it->second.begin(); prop_it != layer_it->second.end(); prop_it++) {
            if (prop_it->second) {
                return true;
            }
        }
    }

    return false;
}

void Style::updateTransitions(time now) {
    uv::writelock lock(mtx);

    for (auto layer_it = transitions.begin(); layer_it != transitions.end(); layer_it++) {
        for (auto prop_it = layer_it->second.begin(); prop_it != layer_it->second.end();/**/) {
            std::shared_ptr<util::transition>& transition = prop_it->second;
            if (transition->update(now) == util::transition::complete) {
                prop_it = transitions[layer_it->first].erase(prop_it);
            } else {
                prop_it++;
            }
        }
    }
}

void Style::cancelTransitions() {
    uv::writelock lock(mtx);

    transitions.clear();
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

