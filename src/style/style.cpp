#include <llmr/style/style.hpp>
#include <llmr/style/style_parser.hpp>
#include <llmr/util/constants.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>


using namespace llmr;

Style::Style() {
    appliedClasses.insert("default");
}

void Style::reset() {
    computed.fills.clear();
    computed.lines.clear();
    computed.icons.clear();
    computed.texts.clear();
    computed.rasters.clear();
    computed.background.color = {{ 1, 1, 1, 1 }};
    computed.background.opacity = 1.0;

    properties_to_transition.clear();
}

void Style::cascade(float z) {
    uv::writelock lock(mtx);

    time start = util::now();

    previous.fills = computed.fills;
    previous.lines = computed.lines;
    previous.icons = computed.icons;
    previous.texts = computed.texts;
    previous.rasters = computed.rasters;
    previous.background = computed.background;

    previous.effective_classes = computed.effective_classes;

    reset();

    // Accomodate for different tile size.
    // TODO: Make this per-layer once individual layers have a specific tile size.
    z += std::log(util::tileSize / 256.0f) / M_LN2;

    // Recalculate style with basic cascading. Also store the last applied class
    // for each property to assist in determining transitions.
    for (const auto& class_pair : classes) {
        const std::string& class_name = class_pair.first;
        const ClassDescription& sheetClass = class_pair.second;

        // Skip if not enabled.
        if (appliedClasses.find(class_name) == appliedClasses.end()) continue;

        // Cascade fill classes.
        for (const auto& fill_pair : sheetClass.fill) {
            const std::string& layer_name = fill_pair.first;
            const llmr::FillClass& layer = fill_pair.second;

            llmr::FillProperties& fill = computed.fills[layer_name];

            if (layer.specifiers.count("enabled")) {
                fill.enabled = layer.enabled.evaluate<bool>(z);
            }

            if (layer.specifiers.count("translate")) {
                fill.translate = {{ layer.translate[0].evaluate<float>(z),
                                    layer.translate[1].evaluate<float>(z) }};
                computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
                if (layer.translate_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Translate] = layer.translate_transition;
                }
            }

            if (layer.specifiers.count("translate-anchor")) {
                fill.translateAnchor = layer.translateAnchor;
            }

            if (layer.specifiers.count("winding")) {
                fill.winding = layer.winding;
            }

            if (layer.specifiers.count("antialias")) {
                fill.antialias = layer.antialias.evaluate<bool>(z);
            }

            if (layer.specifiers.count("color")) {
                fill.fill_color = layer.fill_color;
                computed.effective_classes[layer_name][TransitionablePropertyKey::FillColor] = class_name;
                if (layer.fill_color_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::FillColor] = layer.fill_color_transition;
                }
            }

            if (layer.specifiers.count("stroke")) {
                fill.stroke_color = layer.stroke_color;
                computed.effective_classes[layer_name][TransitionablePropertyKey::StrokeColor] = class_name;
                if (layer.stroke_color_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::StrokeColor] = layer.stroke_color_transition;
                }
            }

            if (layer.specifiers.count("opacity")) {
                fill.opacity = layer.opacity.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
                if (layer.opacity_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Opacity] = layer.opacity_transition;
                }
            }

            if (layer.specifiers.count("image")) {
                fill.image = layer.image;
            }
        }

        // Cascade line classes.
        for (const auto& line_pair : sheetClass.line) {
            const std::string& layer_name = line_pair.first;
            const llmr::LineClass& layer = line_pair.second;

            llmr::LineProperties& stroke = computed.lines[layer_name];

            if (layer.specifiers.count("enabled")) {
                stroke.enabled = layer.enabled.evaluate<bool>(z);
            }

            if (layer.specifiers.count("translate")) {
                stroke.translate = {{ layer.translate[0].evaluate<float>(z),
                                      layer.translate[1].evaluate<float>(z) }};
                computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
                if (layer.translate_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Translate] = layer.translate_transition;
                }
            }

            if (layer.specifiers.count("translate-anchor")) {
                stroke.translateAnchor = layer.translateAnchor;
            }

            if (layer.specifiers.count("width")) {
                stroke.width = layer.width.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Width] = class_name;
                if (layer.width_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Width] = layer.width_transition;
                }

            }

            if (layer.specifiers.count("offset")) {
                stroke.offset = layer.offset.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Offset] = class_name;
                if (layer.offset_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Offset] = layer.offset_transition;
                }
            }

            if (layer.specifiers.count("color")) {
                stroke.color = layer.color;
                computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
                if (layer.color_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Color] = layer.color_transition;
                }
            }

            if (layer.specifiers.count("dasharray")) {
                stroke.dash_array = {{ layer.dash_array[0].evaluate<float>(z),
                                       layer.dash_array[1].evaluate<float>(z) }};
                computed.effective_classes[layer_name][TransitionablePropertyKey::DashArray] = class_name;
                if (layer.dash_array_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::DashArray] = layer.dash_array_transition;
                }
            }

            if (layer.specifiers.count("opacity")) {
                stroke.opacity = layer.opacity.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
                if (layer.opacity_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Opacity] = layer.opacity_transition;
                }
            }
        }

        // Cascade icon classes.
        for (const auto& icon_pair : sheetClass.icon) {
            const std::string& layer_name = icon_pair.first;
            const llmr::IconClass& layer = icon_pair.second;

            llmr::IconProperties& icon = computed.icons[layer_name];

            if (layer.specifiers.count("enabled")) {
                icon.enabled = layer.enabled.evaluate<bool>(z);
            }

            if (layer.specifiers.count("translate")) {
                icon.translate = {{ layer.translate[0].evaluate<float>(z),
                                    layer.translate[1].evaluate<float>(z) }};
                computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
                if (layer.translate_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Translate] = layer.translate_transition;
                }
            }

            if (layer.specifiers.count("translate-anchor")) {
                icon.translateAnchor = layer.translateAnchor;
            }

            if (layer.specifiers.count("color")) {
                icon.color = layer.color;
                computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
                if (layer.color_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Color] = layer.color_transition;
                }
            }

            if (layer.specifiers.count("size")) {
                icon.size = layer.size.evaluate<float>(z);
            }

            if (layer.specifiers.count("opacity")) {
                icon.opacity = layer.opacity.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
                if (layer.opacity_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Opacity] = layer.opacity_transition;
                }
            }

            if (layer.specifiers.count("image")) {
                icon.image = layer.image;
            }

            if (layer.specifiers.count("radius")) {
                icon.radius = layer.radius.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Radius] = class_name;
                if (layer.radius_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Radius] = layer.radius_transition;
                }
            }

            if (layer.specifiers.count("blur")) {
                icon.blur = layer.blur.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Blur] = class_name;
                if (layer.blur_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Blur] = layer.blur_transition;
                }
            }
        }

        // Cascade text classes.
        for (const auto& text_pair : sheetClass.text) {
            const std::string& layer_name = text_pair.first;
            const llmr::TextClass& layer = text_pair.second;

            llmr::TextProperties& text = computed.texts[layer_name];

            if (layer.specifiers.count("enabled")) {
                text.enabled = layer.enabled.evaluate<bool>(z);
            }

            if (layer.specifiers.count("translate")) {
                text.translate = {{ layer.translate[0].evaluate<float>(z),
                                    layer.translate[1].evaluate<float>(z) }};
                computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
                if (layer.translate_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Translate] = layer.translate_transition;
                }
            }

            if (layer.specifiers.count("translate-anchor")) {
                text.translateAnchor = layer.translateAnchor;
            }

            if (layer.specifiers.count("color")) {
                text.color = layer.color;
                computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
                if (layer.color_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Color] = layer.color_transition;
                }
            }

            if (layer.specifiers.count("size")) {
                text.size = layer.size.evaluate<float>(z);
            }

            if (layer.specifiers.count("stroke")) {
                text.halo = layer.halo;
                computed.effective_classes[layer_name][TransitionablePropertyKey::Halo] = class_name;
                if (layer.halo_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Halo] = layer.halo_transition;
                }
            }

            if (layer.specifiers.count("strokeWidth")) {
                text.halo_radius = layer.halo_radius.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::HaloRadius] = class_name;
                if (layer.halo_radius_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::HaloRadius] = layer.halo_radius_transition;
                }
            }

            if (layer.specifiers.count("strokeBlur")) {
                text.halo_blur = layer.halo_blur.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::HaloBlur] = class_name;
                if (layer.halo_blur_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::HaloBlur] = layer.halo_blur_transition;
                }
            }

            if (layer.specifiers.count("rotate")) {
                text.rotate = layer.rotate.evaluate<float>(z);
            }

            if (layer.specifiers.count("alwaysVisible")) {
                text.always_visible = layer.always_visible.evaluate<bool>(z);
            }

            if (layer.specifiers.count("opacity")) {
                text.opacity = layer.opacity.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
                if (layer.opacity_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Opacity] = layer.opacity_transition;
                }
            }
        }

        // Cascade raster classes.
        for (const auto& raster_pair : sheetClass.raster) {
            const std::string& layer_name = raster_pair.first;
            const llmr::RasterClass& layer = raster_pair.second;

            llmr::RasterProperties& raster = computed.rasters[layer_name];

            if (layer.specifiers.count("enabled")) {
                raster.enabled = layer.enabled.evaluate<bool>(z);
            }

            if (layer.specifiers.count("opacity")) {
                raster.opacity = layer.opacity.evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
                if (layer.opacity_transition.duration) {
                    properties_to_transition[layer_name][TransitionablePropertyKey::Opacity] = layer.opacity_transition;
                }
            }
        }

        // Cascade background.
        if (sheetClass.background.specifiers.count("color")) {
            computed.background.color = sheetClass.background.color;
            computed.effective_classes["background"][TransitionablePropertyKey::Color] = class_name;
            if (sheetClass.background.color_transition.duration) {
                properties_to_transition["background"][TransitionablePropertyKey::Color] = sheetClass.background.color_transition;
            }
        }
        if (sheetClass.background.specifiers.count("opacity")) {
            computed.background.opacity = sheetClass.background.opacity.evaluate<float>(z);
            computed.effective_classes["background"][TransitionablePropertyKey::Opacity] = class_name;
            if (sheetClass.background.opacity_transition.duration) {
                properties_to_transition["background"][TransitionablePropertyKey::Opacity] = sheetClass.background.opacity_transition;
            }
        }
    }

    // Apply transitions after the first time.
    if (!initial_render_complete) {
        initial_render_complete = true;
        return;
    }

    // Fills
    for (const auto& fill_pair : computed.fills) {
        const std::string& layer_name = fill_pair.first;

        // translate
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Translate)) {

            computed.fills[layer_name].translate = transitioning.fills[layer_name].translate;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Translate)) {

            transitioning.fills[layer_name].translate = previous.fills[layer_name].translate;

            std::vector<float> from, to, transitioning_ref;
            from.push_back(previous.fills[layer_name].translate[0]);
            from.push_back(previous.fills[layer_name].translate[1]);
            to.push_back(computed.fills[layer_name].translate[0]);
            to.push_back(computed.fills[layer_name].translate[1]);
            transitioning_ref.push_back(transitioning.fills[layer_name].translate[0]);
            transitioning_ref.push_back(transitioning.fills[layer_name].translate[1]);

            transitions[layer_name][TransitionablePropertyKey::Translate] =
                std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                            to,
                                                                            transitioning_ref,
                                                                            start,
                                                                            transitionDuration(layer_name, TransitionablePropertyKey::Translate));

            computed.fills[layer_name].translate = transitioning.fills[layer_name].translate;
        }

        // fill color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::FillColor)) {

            computed.fills[layer_name].fill_color = transitioning.fills[layer_name].fill_color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::FillColor)) {

            transitioning.fills[layer_name].fill_color = previous.fills[layer_name].fill_color;

            transitions[layer_name][TransitionablePropertyKey::FillColor] =
                std::make_shared<util::ease_transition<Color>>(previous.fills[layer_name].fill_color,
                                                               computed.fills[layer_name].fill_color,
                                                               transitioning.fills[layer_name].fill_color,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::FillColor));

            computed.fills[layer_name].fill_color = transitioning.fills[layer_name].fill_color;
        }

        // stroke color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::StrokeColor)) {

            computed.fills[layer_name].stroke_color = transitioning.fills[layer_name].stroke_color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::StrokeColor)) {

            transitioning.fills[layer_name].stroke_color = previous.fills[layer_name].stroke_color;

            transitions[layer_name][TransitionablePropertyKey::StrokeColor] =
                std::make_shared<util::ease_transition<Color>>(previous.fills[layer_name].stroke_color,
                                                               computed.fills[layer_name].stroke_color,
                                                               transitioning.fills[layer_name].stroke_color,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::StrokeColor));

            computed.fills[layer_name].stroke_color = transitioning.fills[layer_name].stroke_color;
        }

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity)) {

            computed.fills[layer_name].opacity = transitioning.fills[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.fills[layer_name].opacity = previous.fills[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.fills[layer_name].opacity,
                                                               computed.fills[layer_name].opacity,
                                                               transitioning.fills[layer_name].opacity,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.fills[layer_name].opacity = transitioning.fills[layer_name].opacity;
        }
    }

    // Lines
    for (const auto& line_pair : computed.lines) {
        const std::string& layer_name = line_pair.first;

        // translate
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Translate)) {

            computed.lines[layer_name].translate = transitioning.lines[layer_name].translate;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Translate)) {

            transitioning.lines[layer_name].translate = previous.lines[layer_name].translate;

            std::vector<float> from, to, transitioning_ref;
            from.push_back(previous.lines[layer_name].translate[0]);
            from.push_back(previous.lines[layer_name].translate[1]);
            to.push_back(computed.lines[layer_name].translate[0]);
            to.push_back(computed.lines[layer_name].translate[1]);
            transitioning_ref.push_back(transitioning.lines[layer_name].translate[0]);
            transitioning_ref.push_back(transitioning.lines[layer_name].translate[1]);

            transitions[layer_name][TransitionablePropertyKey::Translate] =
                std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                            to,
                                                                            transitioning_ref,
                                                                            start,
                                                                            transitionDuration(layer_name, TransitionablePropertyKey::Translate));

            computed.lines[layer_name].translate = transitioning.lines[layer_name].translate;
        }

        // width
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Width)) {

            computed.lines[layer_name].width = transitioning.lines[layer_name].width;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Width)) {

            transitioning.lines[layer_name].width = previous.lines[layer_name].width;

            transitions[layer_name][TransitionablePropertyKey::Width] =
                std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].width,
                                                               computed.lines[layer_name].width,
                                                               transitioning.lines[layer_name].width,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Width));

            computed.lines[layer_name].width = transitioning.lines[layer_name].width;
        }

        // offset
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Offset)) {

            computed.lines[layer_name].offset = transitioning.lines[layer_name].offset;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Offset)) {

            transitioning.lines[layer_name].offset = previous.lines[layer_name].offset;

            transitions[layer_name][TransitionablePropertyKey::Offset] =
                std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].offset,
                                                               computed.lines[layer_name].offset,
                                                               transitioning.lines[layer_name].offset,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Offset));

            computed.lines[layer_name].offset = transitioning.lines[layer_name].offset;
        }

        // color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Color)) {

            computed.lines[layer_name].color = transitioning.lines[layer_name].color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Color)) {

            transitioning.lines[layer_name].color = previous.lines[layer_name].color;

            transitions[layer_name][TransitionablePropertyKey::Color] =
                std::make_shared<util::ease_transition<Color>>(previous.lines[layer_name].color,
                                                               computed.lines[layer_name].color,
                                                               transitioning.lines[layer_name].color,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Color));

            computed.lines[layer_name].color = transitioning.lines[layer_name].color;
        }

        // dasharray
        if (transitionInProgress(layer_name, TransitionablePropertyKey::DashArray)) {

            computed.lines[layer_name].dash_array = transitioning.lines[layer_name].dash_array;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::DashArray)) {

            transitioning.lines[layer_name].dash_array = previous.lines[layer_name].dash_array;

            std::vector<float> from, to, transitioning_ref;
            from.push_back(previous.lines[layer_name].dash_array[0]);
            from.push_back(previous.lines[layer_name].dash_array[1]);
            to.push_back(computed.lines[layer_name].dash_array[0]);
            to.push_back(computed.lines[layer_name].dash_array[1]);
            transitioning_ref.push_back(transitioning.lines[layer_name].dash_array[0]);
            transitioning_ref.push_back(transitioning.lines[layer_name].dash_array[1]);

            transitions[layer_name][TransitionablePropertyKey::DashArray] =
                std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                            to,
                                                                            transitioning_ref,
                                                                            start,
                                                                            transitionDuration(layer_name, TransitionablePropertyKey::DashArray));

            computed.lines[layer_name].dash_array = transitioning.lines[layer_name].dash_array;
        }

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity)) {

            computed.lines[layer_name].opacity = transitioning.lines[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.lines[layer_name].opacity = previous.lines[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].opacity,
                                                               computed.lines[layer_name].opacity,
                                                               transitioning.lines[layer_name].opacity,
                                                               start,
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.lines[layer_name].opacity = transitioning.lines[layer_name].opacity;
        }
    }

    // Icons

    // translate
    // color
    // opacity
    // radius
    // blur

    // Text

    // translate
    // color
    // halo
    // haloradius
    // haloblur
    // opacity

    // Rasters

    // opacity

    // Background

    // color
    // opacity
}

bool Style::transitionInProgress(std::string layer_name, TransitionablePropertyKey key) {
    if (!transitionExists(layer_name, key)) return false;

    return (transitions[layer_name].find(key)->second->update(util::now()) != util::transition::complete);
}

bool Style::transitionExists(std::string layer_name, TransitionablePropertyKey key) {
    return (transitions[layer_name].count(key) != 0);
}

bool Style::inNeedOfTransition(std::string layer_name, TransitionablePropertyKey key) {
    if (!transitionDuration(layer_name, key)) return false;
    if (transitionExists(layer_name, key)) return false;

    return (computed.effective_classes[layer_name][key] != previous.effective_classes[layer_name][key]);
}

uint64_t Style::transitionDuration(std::string layer_name, TransitionablePropertyKey key) {
    return (properties_to_transition[layer_name].count(key) ?
                properties_to_transition[layer_name][key].duration :
                default_transition_duration) * 1_millisecond;
}

bool Style::needsTransition() const {
    uv::readlock lock(mtx);

    for (auto layer_it = transitions.begin(); layer_it != transitions.end(); layer_it++) {
        auto& layer_transitions = layer_it->second;
        if (layer_transitions.size()) {
            return true;
        }
    }

    return false;
}

void Style::updateTransitions(time now) {
    uv::writelock lock(mtx);

    for (auto layer_it = transitions.begin(); layer_it != transitions.end(); layer_it++) {
        auto& layer_transitions = layer_it->second;
        for (auto prop_it = layer_transitions.begin(); prop_it != layer_transitions.end();/**/) {
            auto& transition = prop_it->second;
            if (transition->update(now) == util::transition::complete) {
                prop_it = layer_transitions.erase(prop_it);
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

void Style::setDefaultTransitionDuration(uint64_t duration) {
    default_transition_duration = duration;
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

