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
    previous.lines = computed.lines;
    previous.points = computed.points;
    previous.texts = computed.texts;
    previous.rasters = computed.rasters;

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

            // enabled
            stroke.enabled = layer.enabled.evaluate<bool>(z);

            // translate (transitionable)
            if (layer.translate_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Translate) &&
                (layer.translate[0].evaluate<float>(z) != previous.lines[layer_name].translate[0] ||
                 layer.translate[1].evaluate<float>(z) != previous.lines[layer_name].translate[1])) {

                transitioning.lines[layer_name].translate = {{ previous.lines[layer_name].translate[0],
                                                               previous.lines[layer_name].translate[1] }};

                std::vector<float> from, to, transitioning_ref;
                from.push_back(previous.lines[layer_name].translate[0]);
                from.push_back(previous.lines[layer_name].translate[1]);
                to.push_back(layer.translate[0].evaluate<float>(z));
                to.push_back(layer.translate[1].evaluate<float>(z));
                transitioning_ref.push_back(transitioning.lines[layer_name].translate[0]);
                transitioning_ref.push_back(transitioning.lines[layer_name].translate[1]);
                transitions[layer_name][PropertyKey::Translate] = std::make_shared<util::ease_transition<std::vector<float>>>(
                                                                      from,
                                                                      to,
                                                                      transitioning_ref,
                                                                      start,
                                                                      layer.translate_transition.duration * 1_millisecond
                                                                  );
            } else if (transitions[layer_name].count(PropertyKey::Translate)) {
                stroke.translate = transitioning.lines[layer_name].translate;
            } else {
                stroke.translate = {{ layer.translate[0].evaluate<float>(z), layer.translate[1].evaluate<float>(z) }};
            }

            // translate anchor
            stroke.translateAnchor = layer.translateAnchor;

            // width (transitionable)
            if (layer.width_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Width) &&
                layer.width.evaluate<float>(z) != previous.lines[layer_name].width) {

                transitioning.lines[layer_name].width = previous.lines[layer_name].width;

                transitions[layer_name][PropertyKey::Width] = std::make_shared<util::ease_transition<float>> (
                                                                  previous.lines[layer_name].width,
                                                                  layer.width.evaluate<float>(z),
                                                                  transitioning.lines[layer_name].width,
                                                                  start,
                                                                  layer.width_transition.duration * 1_millisecond
                                                              );
            } else if (transitions[layer_name].count(PropertyKey::Width)) {
                stroke.width = transitioning.lines[layer_name].width;
            } else {
                stroke.width = layer.width.evaluate<float>(z);
            }

            // offset (transitionable)
            if (layer.offset_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Offset) &&
                layer.offset.evaluate<float>(z) != previous.lines[layer_name].offset) {

                transitioning.lines[layer_name].offset = previous.lines[layer_name].offset;

                transitions[layer_name][PropertyKey::Offset] = std::make_shared<util::ease_transition<float>> (
                                                                   previous.lines[layer_name].offset,
                                                                   layer.offset.evaluate<float>(z),
                                                                   transitioning.lines[layer_name].offset,
                                                                   start,
                                                                   layer.offset_transition.duration * 1_millisecond
                                                               );
            } else if (transitions[layer_name].count(PropertyKey::Offset)) {
                stroke.offset = transitioning.lines[layer_name].offset;
            } else {
                stroke.offset = layer.offset.evaluate<float>(z);
            }

            // color (transitionable)
            if (layer.color_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Color) &&
                layer.color != previous.lines[layer_name].color) {

                transitioning.lines[layer_name].color = previous.lines[layer_name].color;

                transitions[layer_name][PropertyKey::Color] = std::make_shared<util::ease_transition<Color>>(
                                                                  previous.lines[layer_name].color,
                                                                  layer.color,
                                                                  transitioning.lines[layer_name].color,
                                                                  start,
                                                                  layer.color_transition.duration * 1_millisecond
                                                              );
            } else if (transitions[layer_name].count(PropertyKey::Color)) {
                stroke.color = transitioning.lines[layer_name].color;
            }
            else {
                stroke.color = layer.color;
            }

            // dash array (transitionable)
            if (layer.dash_array_transition.duration &&
                !transitions[layer_name].count(PropertyKey::DashArray) &&
                (layer.dash_array[0].evaluate<float>(z) != previous.lines[layer_name].dash_array[0] ||
                 layer.dash_array[1].evaluate<float>(z) != previous.lines[layer_name].dash_array[1])) {

                transitioning.lines[layer_name].dash_array = {{ previous.lines[layer_name].dash_array[0],
                                                                previous.lines[layer_name].dash_array[1] }};

                std::vector<float> from, to, transitioning_ref;
                from.push_back(previous.lines[layer_name].dash_array[0]);
                from.push_back(previous.lines[layer_name].dash_array[1]);
                to.push_back(layer.dash_array[0].evaluate<float>(z));
                to.push_back(layer.dash_array[1].evaluate<float>(z));
                transitioning_ref.push_back(transitioning.lines[layer_name].dash_array[0]);
                transitioning_ref.push_back(transitioning.lines[layer_name].dash_array[1]);
                transitions[layer_name][PropertyKey::DashArray] = std::make_shared<util::ease_transition<std::vector<float>>>(
                                                                      from,
                                                                      to,
                                                                      transitioning_ref,
                                                                      start,
                                                                      layer.translate_transition.duration * 1_millisecond
                                                                  );
            } else if (transitions[layer_name].count(PropertyKey::DashArray)) {
                stroke.dash_array = transitioning.lines[layer_name].dash_array;
            } else {
                stroke.dash_array = {{ layer.dash_array[0].evaluate<float>(z), layer.dash_array[1].evaluate<float>(z) }};
            }

            // opacity (transitionable)
            if (layer.opacity_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Opacity) &&
                layer.opacity.evaluate<float>(z) != previous.lines[layer_name].opacity) {

                transitioning.lines[layer_name].opacity = previous.lines[layer_name].opacity;

                transitions[layer_name][PropertyKey::Opacity] = std::make_shared<util::ease_transition<float>>(
                                                                    previous.lines[layer_name].opacity,
                                                                    layer.opacity.evaluate<float>(z),
                                                                    transitioning.lines[layer_name].opacity,
                                                                    start,
                                                                    layer.opacity_transition.duration * 1_millisecond
                                                                );
            } else if (transitions[layer_name].count(PropertyKey::Opacity)) {
                stroke.opacity = transitioning.lines[layer_name].opacity;
            } else {
                stroke.opacity = layer.opacity.evaluate<float>(z);
            }
        }

        // Cascade point classes
        for (const auto& point_pair : sheetClass.point) {
            const std::string& layer_name = point_pair.first;
            const llmr::PointClass& layer = point_pair.second;

            // TODO: This should be restricted to point styles that have actual
            // values so as to not override with default values.
            llmr::PointProperties& point = computed.points[layer_name];

            // enabled
            point.enabled = layer.enabled.evaluate<bool>(z);

            // translate (transitionable)
            if (layer.translate_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Translate) &&
                (layer.translate[0].evaluate<float>(z) != previous.points[layer_name].translate[0] ||
                 layer.translate[1].evaluate<float>(z) != previous.points[layer_name].translate[1])) {

                transitioning.points[layer_name].translate = {{ previous.points[layer_name].translate[0],
                                                                previous.points[layer_name].translate[1] }};

                std::vector<float> from, to, transitioning_ref;
                from.push_back(previous.points[layer_name].translate[0]);
                from.push_back(previous.points[layer_name].translate[1]);
                to.push_back(layer.translate[0].evaluate<float>(z));
                to.push_back(layer.translate[1].evaluate<float>(z));
                transitioning_ref.push_back(transitioning.points[layer_name].translate[0]);
                transitioning_ref.push_back(transitioning.points[layer_name].translate[1]);
                transitions[layer_name][PropertyKey::Translate] = std::make_shared<util::ease_transition<std::vector<float>>>(
                                                                      from,
                                                                      to,
                                                                      transitioning_ref,
                                                                      start,
                                                                      layer.translate_transition.duration * 1_millisecond
                                                                  );
            } else if (transitions[layer_name].count(PropertyKey::Translate)) {
                point.translate = transitioning.points[layer_name].translate;
            } else {
                point.translate = {{ layer.translate[0].evaluate<float>(z), layer.translate[1].evaluate<float>(z) }};
            }

            // translate anchor
            point.translateAnchor = layer.translateAnchor;

            // color (transitionable)
            if (layer.color_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Color) &&
                layer.color != previous.points[layer_name].color) {

                transitioning.points[layer_name].color = previous.points[layer_name].color;

                transitions[layer_name][PropertyKey::Color] = std::make_shared<util::ease_transition<Color>>(
                                                                  previous.points[layer_name].color,
                                                                  layer.color,
                                                                  transitioning.points[layer_name].color,
                                                                  start,
                                                                  layer.color_transition.duration * 1_millisecond
                                                              );
            } else if (transitions[layer_name].count(PropertyKey::Color)) {
                point.color = transitioning.points[layer_name].color;
            }
            else {
                point.color = layer.color;
            }

            // size
            point.size = layer.size.evaluate<float>(z);

            // opacity (transitionable)
            if (layer.opacity_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Opacity) &&
                layer.opacity.evaluate<float>(z) != previous.points[layer_name].opacity) {

                transitioning.points[layer_name].opacity = previous.points[layer_name].opacity;

                transitions[layer_name][PropertyKey::Opacity] = std::make_shared<util::ease_transition<float>>(
                                                                    previous.points[layer_name].opacity,
                                                                    layer.opacity.evaluate<float>(z),
                                                                    transitioning.points[layer_name].opacity,
                                                                    start,
                                                                    layer.opacity_transition.duration * 1_millisecond
                                                                );
            } else if (transitions[layer_name].count(PropertyKey::Opacity)) {
                point.opacity = transitioning.points[layer_name].opacity;
            } else {
                point.opacity = layer.opacity.evaluate<float>(z);
            }

            // image
            point.image = layer.image;

            // radius (transitionable)
            if (layer.radius_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Radius) &&
                layer.radius.evaluate<float>(z) != previous.points[layer_name].radius) {

                transitioning.points[layer_name].radius = previous.points[layer_name].radius;

                transitions[layer_name][PropertyKey::Radius] = std::make_shared<util::ease_transition<float>>(
                                                                   previous.points[layer_name].radius,
                                                                   layer.radius.evaluate<float>(z),
                                                                   transitioning.points[layer_name].radius,
                                                                   start,
                                                                   layer.radius_transition.duration * 1_millisecond
                                                               );
            } else if (transitions[layer_name].count(PropertyKey::Radius)) {
                point.radius = transitioning.points[layer_name].radius;
            } else {
                point.radius = layer.radius.evaluate<float>(z);
            }

            // blur (transitionable)
            if (layer.blur_transition.duration &&
                !transitions[layer_name].count(PropertyKey::Blur) &&
                layer.blur.evaluate<float>(z) != previous.points[layer_name].blur) {

                transitioning.points[layer_name].blur = previous.points[layer_name].blur;

                transitions[layer_name][PropertyKey::Blur] = std::make_shared<util::ease_transition<float>>(
                                                                 previous.points[layer_name].blur,
                                                                 layer.blur.evaluate<float>(z),
                                                                 transitioning.points[layer_name].blur,
                                                                 start,
                                                                 layer.blur_transition.duration * 1_millisecond
                                                             );
            } else if (transitions[layer_name].count(PropertyKey::Blur)) {
                point.blur = transitioning.points[layer_name].blur;
            } else {
                point.blur = layer.blur.evaluate<float>(z);
            }
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

