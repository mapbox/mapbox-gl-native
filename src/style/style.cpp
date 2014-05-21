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

    reset();

    std::set<std::string> currentPassAppliedClasses;

    // Accomodate for different tile size.
    // TODO: Make this per-layer once individual layers have a specific tile size.
    z += std::log(util::tileSize / 256.0f) / M_LN2;

    // Recalculate style
    // Basic cascading
    for (const auto& class_pair : classes) {
        const std::string& class_name = class_pair.first;
        const ClassDescription& sheetClass = class_pair.second;

        // Not enabled
        if (appliedClasses.find(class_name) == appliedClasses.end()) continue;
        currentPassAppliedClasses.insert(class_name);

        // Cascade fill classes
        for (const auto& fill_pair : sheetClass.fill) {
            const std::string& layer_name = fill_pair.first;
            const llmr::FillClass& layer = fill_pair.second;

            llmr::FillProperties& fill = computed.fills[layer_name];

            // enabled
            if (layer.specifiers.count("enabled")) {
                fill.enabled = layer.enabled.evaluate<bool>(z);
            }

            // translate (transitionable)
            if (layer.specifiers.count("translate")) {
                if (layer.translate_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Translate) &&
                    (layer.translate[0].evaluate<float>(z) != previous.fills[layer_name].translate[0] ||
                     layer.translate[1].evaluate<float>(z) != previous.fills[layer_name].translate[1]) &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.fills[layer_name].translate = {{ previous.fills[layer_name].translate[0],
                                                                   previous.fills[layer_name].translate[1] }};

                    std::vector<float> from, to, transitioning_ref;
                    from.push_back(previous.fills[layer_name].translate[0]);
                    from.push_back(previous.fills[layer_name].translate[1]);
                    to.push_back(layer.translate[0].evaluate<float>(z));
                    to.push_back(layer.translate[1].evaluate<float>(z));
                    transitioning_ref.push_back(transitioning.fills[layer_name].translate[0]);
                    transitioning_ref.push_back(transitioning.fills[layer_name].translate[1]);
                    transitions[layer_name][TransitionablePropertyKey::Translate] =
                        std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                                    to,
                                                                                    transitioning_ref,
                                                                                    start,
                                                                                    layer.translate_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Translate)) {
                    fill.translate = transitioning.fills[layer_name].translate;
                } else {
                    fill.translate = {{ layer.translate[0].evaluate<float>(z),
                                        layer.translate[1].evaluate<float>(z) }};
                }
            }

            // translate anchor
            if (layer.specifiers.count("translate-anchor")) {
                fill.translateAnchor = layer.translateAnchor;
            }

            // winding
            fill.winding = layer.winding;

            // antialias
            if (layer.specifiers.count("antialias")) {
                fill.antialias = layer.antialias.evaluate<bool>(z);
            }

            // fill color (transitionable)
            if (layer.specifiers.count("color")) {
                if (layer.fill_color_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::FillColor) &&
                    layer.fill_color != previous.fills[layer_name].fill_color &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.fills[layer_name].fill_color = previous.fills[layer_name].fill_color;

                    transitions[layer_name][TransitionablePropertyKey::FillColor] =
                        std::make_shared<util::ease_transition<Color>>(previous.fills[layer_name].fill_color,
                                                                       layer.fill_color,
                                                                       transitioning.fills[layer_name].fill_color,
                                                                       start,
                                                                       layer.fill_color_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::FillColor)) {
                    fill.fill_color = transitioning.fills[layer_name].fill_color;
                }
                else {
                    fill.fill_color = layer.fill_color;
                }
            }

            // stroke color (transitionable)
            if (layer.specifiers.count("stroke")) {
                if (layer.stroke_color_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::StrokeColor) &&
                    layer.stroke_color != previous.fills[layer_name].stroke_color &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.fills[layer_name].stroke_color = previous.fills[layer_name].stroke_color;

                    transitions[layer_name][TransitionablePropertyKey::StrokeColor] =
                        std::make_shared<util::ease_transition<Color>>(previous.fills[layer_name].stroke_color,
                                                                       layer.stroke_color,
                                                                       transitioning.fills[layer_name].stroke_color,
                                                                       start,
                                                                       layer.stroke_color_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::StrokeColor)) {
                    fill.stroke_color = transitioning.fills[layer_name].stroke_color;
                }
                else {
                    fill.stroke_color = layer.stroke_color;
                }
            }

            // opacity (transitionable)
            if (layer.specifiers.count("opacity")) {
                if (layer.opacity_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Opacity) &&
                    layer.opacity.evaluate<float>(z) != previous.fills[layer_name].opacity &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.fills[layer_name].opacity = previous.fills[layer_name].opacity;

                    transitions[layer_name][TransitionablePropertyKey::Opacity] =
                        std::make_shared<util::ease_transition<float>>(previous.fills[layer_name].opacity,
                                                                       layer.opacity.evaluate<float>(z),
                                                                       transitioning.fills[layer_name].opacity,
                                                                       start,
                                                                       layer.opacity_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Opacity)) {
                    fill.opacity = transitioning.fills[layer_name].opacity;
                } else {
                    fill.opacity = layer.opacity.evaluate<float>(z);
                }
            }

            // image
            if (layer.specifiers.count("image")) {
                fill.image = layer.image;
            }
        }

        // Cascade line classes
        for (const auto& line_pair : sheetClass.line) {
            const std::string& layer_name = line_pair.first;
            const llmr::LineClass& layer = line_pair.second;

            llmr::LineProperties& stroke = computed.lines[layer_name];

            // enabled
            if (layer.specifiers.count("enabled")) {
                stroke.enabled = layer.enabled.evaluate<bool>(z);
            }

            // translate (transitionable)
            if (layer.specifiers.count("translate")) {
                if (layer.translate_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Translate) &&
                    (layer.translate[0].evaluate<float>(z) != previous.lines[layer_name].translate[0] ||
                     layer.translate[1].evaluate<float>(z) != previous.lines[layer_name].translate[1]) &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].translate = {{ previous.lines[layer_name].translate[0],
                                                                   previous.lines[layer_name].translate[1] }};

                    std::vector<float> from, to, transitioning_ref;
                    from.push_back(previous.lines[layer_name].translate[0]);
                    from.push_back(previous.lines[layer_name].translate[1]);
                    to.push_back(layer.translate[0].evaluate<float>(z));
                    to.push_back(layer.translate[1].evaluate<float>(z));
                    transitioning_ref.push_back(transitioning.lines[layer_name].translate[0]);
                    transitioning_ref.push_back(transitioning.lines[layer_name].translate[1]);
                    transitions[layer_name][TransitionablePropertyKey::Translate] =
                        std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                                    to,
                                                                                    transitioning_ref,
                                                                                    start,
                                                                                    layer.translate_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Translate)) {
                    stroke.translate = transitioning.lines[layer_name].translate;
                } else {
                    stroke.translate = {{ layer.translate[0].evaluate<float>(z),
                                          layer.translate[1].evaluate<float>(z) }};
                }
            }

            // translate anchor
            if (layer.specifiers.count("translate-anchor")) {
                stroke.translateAnchor = layer.translateAnchor;
            }

            // width (transitionable)
            if (layer.specifiers.count("width")) {
                if (layer.width_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Width) &&
                    layer.width.evaluate<float>(z) != previous.lines[layer_name].width &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].width = previous.lines[layer_name].width;

                    transitions[layer_name][TransitionablePropertyKey::Width] =
                        std::make_shared<util::ease_transition<float>> (previous.lines[layer_name].width,
                                                                        layer.width.evaluate<float>(z),
                                                                        transitioning.lines[layer_name].width,
                                                                        start,
                                                                        layer.width_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Width)) {
                    stroke.width = transitioning.lines[layer_name].width;
                } else {
                    stroke.width = layer.width.evaluate<float>(z);
                }
            }

            // offset (transitionable)
            if (layer.specifiers.count("offset")) {
                if (layer.offset_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Offset) &&
                    layer.offset.evaluate<float>(z) != previous.lines[layer_name].offset &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].offset = previous.lines[layer_name].offset;

                    transitions[layer_name][TransitionablePropertyKey::Offset] =
                        std::make_shared<util::ease_transition<float>> (previous.lines[layer_name].offset,
                                                                        layer.offset.evaluate<float>(z),
                                                                        transitioning.lines[layer_name].offset,
                                                                        start,
                                                                        layer.offset_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Offset)) {
                    stroke.offset = transitioning.lines[layer_name].offset;
                } else {
                    stroke.offset = layer.offset.evaluate<float>(z);
                }
            }

            // color (transitionable)
            if (layer.specifiers.count("color")) {
                if (layer.color_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Color) &&
                    layer.color != previous.lines[layer_name].color &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].color = previous.lines[layer_name].color;

                    transitions[layer_name][TransitionablePropertyKey::Color] =
                        std::make_shared<util::ease_transition<Color>>(previous.lines[layer_name].color,
                                                                       layer.color,
                                                                       transitioning.lines[layer_name].color,
                                                                       start,
                                                                       layer.color_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Color)) {
                    stroke.color = transitioning.lines[layer_name].color;
                }
                else {
                    stroke.color = layer.color;
                }
            }

            // dash array (transitionable)
            if (layer.specifiers.count("dasharray")) {
                if (layer.dash_array_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::DashArray) &&
                    (layer.dash_array[0].evaluate<float>(z) != previous.lines[layer_name].dash_array[0] ||
                     layer.dash_array[1].evaluate<float>(z) != previous.lines[layer_name].dash_array[1]) &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].dash_array = {{ previous.lines[layer_name].dash_array[0],
                                                                    previous.lines[layer_name].dash_array[1] }};

                    std::vector<float> from, to, transitioning_ref;
                    from.push_back(previous.lines[layer_name].dash_array[0]);
                    from.push_back(previous.lines[layer_name].dash_array[1]);
                    to.push_back(layer.dash_array[0].evaluate<float>(z));
                    to.push_back(layer.dash_array[1].evaluate<float>(z));
                    transitioning_ref.push_back(transitioning.lines[layer_name].dash_array[0]);
                    transitioning_ref.push_back(transitioning.lines[layer_name].dash_array[1]);
                    transitions[layer_name][TransitionablePropertyKey::DashArray] =
                        std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                                    to,
                                                                                    transitioning_ref,
                                                                                    start,
                                                                                    layer.translate_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::DashArray)) {
                    stroke.dash_array = transitioning.lines[layer_name].dash_array;
                } else {
                    stroke.dash_array = {{ layer.dash_array[0].evaluate<float>(z),
                                           layer.dash_array[1].evaluate<float>(z) }};
                }
            }

            // opacity (transitionable)
            if (layer.specifiers.count("opacity")) {
                if (layer.opacity_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Opacity) &&
                    layer.opacity.evaluate<float>(z) != previous.lines[layer_name].opacity &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].opacity = previous.lines[layer_name].opacity;

                    transitions[layer_name][TransitionablePropertyKey::Opacity] =
                        std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].opacity,
                                                                       layer.opacity.evaluate<float>(z),
                                                                       transitioning.lines[layer_name].opacity,
                                                                       start,
                                                                       layer.opacity_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Opacity)) {
                    stroke.opacity = transitioning.lines[layer_name].opacity;
                } else {
                    stroke.opacity = layer.opacity.evaluate<float>(z);
                }
            }
        }

        // Cascade icon classes
        for (const auto& icon_pair : sheetClass.icon) {
            const std::string& layer_name = icon_pair.first;
            const llmr::IconClass& layer = icon_pair.second;

            llmr::IconProperties& icon = computed.icons[layer_name];

            // enabled
            if (layer.specifiers.count("enabled")) {
                icon.enabled = layer.enabled.evaluate<bool>(z);
            }

            // translate (transitionable)
            if (layer.specifiers.count("translate")) {
                if (layer.translate_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Translate) &&
                    (layer.translate[0].evaluate<float>(z) != previous.icons[layer_name].translate[0] ||
                     layer.translate[1].evaluate<float>(z) != previous.icons[layer_name].translate[1]) &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.icons[layer_name].translate = {{ previous.icons[layer_name].translate[0],
                                                                   previous.icons[layer_name].translate[1] }};

                    std::vector<float> from, to, transitioning_ref;
                    from.push_back(previous.icons[layer_name].translate[0]);
                    from.push_back(previous.icons[layer_name].translate[1]);
                    to.push_back(layer.translate[0].evaluate<float>(z));
                    to.push_back(layer.translate[1].evaluate<float>(z));
                    transitioning_ref.push_back(transitioning.icons[layer_name].translate[0]);
                    transitioning_ref.push_back(transitioning.icons[layer_name].translate[1]);
                    transitions[layer_name][TransitionablePropertyKey::Translate] =
                        std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                                    to,
                                                                                    transitioning_ref,
                                                                                    start,
                                                                                    layer.translate_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Translate)) {
                    icon.translate = transitioning.icons[layer_name].translate;
                } else {
                    icon.translate = {{ layer.translate[0].evaluate<float>(z),
                                        layer.translate[1].evaluate<float>(z) }};
                }
            }

            // translate anchor
            if (layer.specifiers.count("translate-anchor")) {
                icon.translateAnchor = layer.translateAnchor;
            }

            // color (transitionable)
            if (layer.specifiers.count("color")) {
                if (layer.color_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Color) &&
                    layer.color != previous.icons[layer_name].color &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.icons[layer_name].color = previous.icons[layer_name].color;

                    transitions[layer_name][TransitionablePropertyKey::Color] =
                        std::make_shared<util::ease_transition<Color>>(previous.icons[layer_name].color,
                                                                       layer.color,
                                                                       transitioning.icons[layer_name].color,
                                                                       start,
                                                                       layer.color_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Color)) {
                    icon.color = transitioning.icons[layer_name].color;
                }
                else {
                    icon.color = layer.color;
                }
            }

            // size
            if (layer.specifiers.count("size")) {
                icon.size = layer.size.evaluate<float>(z);
            }

            // opacity (transitionable)
            if (layer.specifiers.count("opacity")) {
                if (layer.opacity_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Opacity) &&
                    layer.opacity.evaluate<float>(z) != previous.icons[layer_name].opacity &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.icons[layer_name].opacity = previous.icons[layer_name].opacity;

                    transitions[layer_name][TransitionablePropertyKey::Opacity] =
                        std::make_shared<util::ease_transition<float>>(previous.icons[layer_name].opacity,
                                                                       layer.opacity.evaluate<float>(z),
                                                                       transitioning.icons[layer_name].opacity,
                                                                       start,
                                                                       layer.opacity_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Opacity)) {
                    icon.opacity = transitioning.icons[layer_name].opacity;
                } else {
                    icon.opacity = layer.opacity.evaluate<float>(z);
                }
            }

            // image
            if (layer.specifiers.count("image")) {
                icon.image = layer.image;
            }

            // radius (transitionable)
            if (layer.specifiers.count("radius")) {
                if (layer.radius_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Radius) &&
                    layer.radius.evaluate<float>(z) != previous.icons[layer_name].radius &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.icons[layer_name].radius = previous.icons[layer_name].radius;

                    transitions[layer_name][TransitionablePropertyKey::Radius] =
                        std::make_shared<util::ease_transition<float>>(previous.icons[layer_name].radius,
                                                                       layer.radius.evaluate<float>(z),
                                                                       transitioning.icons[layer_name].radius,
                                                                       start,
                                                                       layer.radius_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Radius)) {
                    icon.radius = transitioning.icons[layer_name].radius;
                } else {
                    icon.radius = layer.radius.evaluate<float>(z);
                }
            }

            // blur (transitionable)
            if (layer.specifiers.count("blur")) {
                if (layer.blur_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Blur) &&
                    layer.blur.evaluate<float>(z) != previous.icons[layer_name].blur &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.icons[layer_name].blur = previous.icons[layer_name].blur;

                    transitions[layer_name][TransitionablePropertyKey::Blur] =
                        std::make_shared<util::ease_transition<float>>(previous.icons[layer_name].blur,
                                                                       layer.blur.evaluate<float>(z),
                                                                       transitioning.icons[layer_name].blur,
                                                                       start,
                                                                       layer.blur_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Blur)) {
                    icon.blur = transitioning.icons[layer_name].blur;
                } else {
                    icon.blur = layer.blur.evaluate<float>(z);
                }
            }
        }

        // Cascade text classes
        for (const auto& text_pair : sheetClass.text) {
            const std::string& layer_name = text_pair.first;
            const llmr::TextClass& layer = text_pair.second;

            llmr::TextProperties& text = computed.texts[layer_name];

            // enabled
            if (layer.specifiers.count("enabled")) {
                text.enabled = layer.enabled.evaluate<bool>(z);
            }

            // translate (transitionable)
            if (layer.specifiers.count("translate")) {
                if (layer.translate_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Translate) &&
                    (layer.translate[0].evaluate<float>(z) != previous.texts[layer_name].translate[0] ||
                     layer.translate[1].evaluate<float>(z) != previous.texts[layer_name].translate[1]) &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.texts[layer_name].translate = {{ previous.texts[layer_name].translate[0],
                                                                   previous.texts[layer_name].translate[1] }};

                    std::vector<float> from, to, transitioning_ref;
                    from.push_back(previous.texts[layer_name].translate[0]);
                    from.push_back(previous.texts[layer_name].translate[1]);
                    to.push_back(layer.translate[0].evaluate<float>(z));
                    to.push_back(layer.translate[1].evaluate<float>(z));
                    transitioning_ref.push_back(transitioning.texts[layer_name].translate[0]);
                    transitioning_ref.push_back(transitioning.texts[layer_name].translate[1]);
                    transitions[layer_name][TransitionablePropertyKey::Translate] =
                        std::make_shared<util::ease_transition<std::vector<float>>>(from,
                                                                                    to,
                                                                                    transitioning_ref,
                                                                                    start,
                                                                                    layer.translate_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Translate)) {
                    text.translate = transitioning.texts[layer_name].translate;
                } else {
                    text.translate = {{ layer.translate[0].evaluate<float>(z),
                                        layer.translate[1].evaluate<float>(z) }};
                }
            }

            // translate anchor
            if (layer.specifiers.count("translate-anchor")) {
                text.translateAnchor = layer.translateAnchor;
            }

            // color (transitionable)
            if (layer.specifiers.count("color")) {
                if (layer.color_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Color) &&
                    layer.color != previous.texts[layer_name].color &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].color = previous.texts[layer_name].color;

                    transitions[layer_name][TransitionablePropertyKey::Color] =
                        std::make_shared<util::ease_transition<Color>>(previous.texts[layer_name].color,
                                                                       layer.color,
                                                                       transitioning.texts[layer_name].color,
                                                                       start,
                                                                       layer.color_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Color)) {
                    text.color = transitioning.texts[layer_name].color;
                }
                else {
                    text.color = layer.color;
                }
            }

            // size
            if (layer.specifiers.count("size")) {
                text.size = layer.size.evaluate<float>(z);
            }

            // halo color (transitionable)
            if (layer.specifiers.count("stroke")) {
                if (layer.halo_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Halo) &&
                    layer.halo != previous.texts[layer_name].halo &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.texts[layer_name].halo = previous.texts[layer_name].halo;

                    transitions[layer_name][TransitionablePropertyKey::Halo] =
                    std::make_shared<util::ease_transition<Color>>(previous.texts[layer_name].halo,
                                                                   layer.halo,
                                                                   transitioning.texts[layer_name].halo,
                                                                   start,
                                                                   layer.halo_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Halo)) {
                    text.halo = transitioning.texts[layer_name].halo;
                }
                else {
                    text.halo = layer.halo;
                }
            }

            // halo radius (transitionable)
            if (layer.specifiers.count("strokeWidth")) {
                if (layer.halo_radius_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::HaloRadius) &&
                    layer.halo_radius.evaluate<float>(z) != previous.texts[layer_name].halo_radius &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.texts[layer_name].halo_radius = previous.texts[layer_name].halo_radius;

                    transitions[layer_name][TransitionablePropertyKey::HaloRadius] =
                        std::make_shared<util::ease_transition<float>> (previous.texts[layer_name].halo_radius,
                                                                        layer.halo_radius.evaluate<float>(z),
                                                                        transitioning.texts[layer_name].halo_radius,
                                                                        start,
                                                                        layer.halo_radius_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::HaloRadius)) {
                    text.halo_radius = transitioning.texts[layer_name].halo_radius;
                } else {
                    text.halo_radius = layer.halo_radius.evaluate<float>(z);
                }
            }

            // halo blur (transitionable)
            if (layer.specifiers.count("strokeBlur")) {
                if (layer.halo_blur_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::HaloBlur) &&
                    layer.halo_blur.evaluate<float>(z) != previous.texts[layer_name].halo_blur &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.texts[layer_name].halo_blur = previous.texts[layer_name].halo_blur;

                    transitions[layer_name][TransitionablePropertyKey::HaloBlur] =
                        std::make_shared<util::ease_transition<float>> (previous.texts[layer_name].halo_blur,
                                                                        layer.halo_blur.evaluate<float>(z),
                                                                        transitioning.texts[layer_name].halo_blur,
                                                                        start,
                                                                        layer.halo_blur_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::HaloBlur)) {
                    text.halo_blur = transitioning.texts[layer_name].halo_blur;
                } else {
                    text.halo_blur = layer.halo_blur.evaluate<float>(z);
                }
            }

            // rotate
            if (layer.specifiers.count("rotate")) {
                text.rotate = layer.rotate.evaluate<float>(z);
            }

            // always visible
            if (layer.specifiers.count("alwaysVisible")) {
                text.always_visible = layer.always_visible.evaluate<bool>(z);
            }

            // opacity (transitionable)
            if (layer.specifiers.count("opacity")) {
                if (layer.opacity_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Opacity) &&
                    layer.opacity.evaluate<float>(z) != previous.texts[layer_name].opacity &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.lines[layer_name].opacity = previous.texts[layer_name].opacity;

                    transitions[layer_name][TransitionablePropertyKey::Opacity] =
                        std::make_shared<util::ease_transition<float>>(previous.texts[layer_name].opacity,
                                                                       layer.opacity.evaluate<float>(z),
                                                                       transitioning.texts[layer_name].opacity,
                                                                       start,
                                                                       layer.opacity_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Opacity)) {
                    text.opacity = transitioning.texts[layer_name].opacity;
                } else {
                    text.opacity = layer.opacity.evaluate<float>(z);
                }
            }
        }

        // Cascade raster classes
        for (const auto& raster_pair : sheetClass.raster) {
            const std::string& layer_name = raster_pair.first;
            const llmr::RasterClass& layer = raster_pair.second;

            llmr::RasterProperties& raster = computed.rasters[layer_name];

            // enabled
            if (layer.specifiers.count("enabled")) {
                raster.enabled = layer.enabled.evaluate<bool>(z);
            }

            // opacity (transitionable)
            if (layer.specifiers.count("opacity")) {
                if (layer.opacity_transition.duration &&
                    !transitions[layer_name].count(TransitionablePropertyKey::Opacity) &&
                    layer.opacity.evaluate<float>(z) != previous.rasters[layer_name].opacity &&
                    !previouslyAppliedClasses.count(class_name)) {

                    transitioning.rasters[layer_name].opacity = previous.rasters[layer_name].opacity;

                    transitions[layer_name][TransitionablePropertyKey::Opacity] =
                        std::make_shared<util::ease_transition<float>>(previous.rasters[layer_name].opacity,
                                                                       layer.opacity.evaluate<float>(z),
                                                                       transitioning.rasters[layer_name].opacity,
                                                                       start,
                                                                       layer.opacity_transition.duration * 1_millisecond);
                } else if (transitions[layer_name].count(TransitionablePropertyKey::Opacity)) {
                    raster.opacity = transitioning.rasters[layer_name].opacity;
                } else {
                    raster.opacity = layer.opacity.evaluate<float>(z);
                }
            }
        }

        // Cascade background

        // color (transitionable)
        if (sheetClass.background.specifiers.count("color")) {
            if (sheetClass.background.color_transition.duration &&
                !transitions["background"].count(TransitionablePropertyKey::Color) &&
                sheetClass.background.color != previous.background.color &&
                !previouslyAppliedClasses.count(class_name)) {

                transitioning.background.color = previous.background.color;

                transitions["background"][TransitionablePropertyKey::Color] =
                    std::make_shared<util::ease_transition<Color>>(previous.background.color,
                                                                   sheetClass.background.color,
                                                                   transitioning.background.color,
                                                                   start,
                                                                   sheetClass.background.color_transition.duration * 1_millisecond);
            } else if (transitions["background"].count(TransitionablePropertyKey::Color)) {
                computed.background.color = transitioning.background.color;
            } else {
                computed.background.color = sheetClass.background.color;
            }
        }

        // opacity (transitionable)
        if (sheetClass.background.specifiers.count("opacity")) {
            if (sheetClass.background.opacity_transition.duration &&
                !transitions["background"].count(TransitionablePropertyKey::Opacity) &&
                sheetClass.background.opacity.evaluate<float>(z) != previous.background.opacity &&
                !previouslyAppliedClasses.count(class_name)) {

                transitioning.background.opacity = previous.background.opacity;

                transitions["background"][TransitionablePropertyKey::Opacity] =
                    std::make_shared<util::ease_transition<float>>(previous.background.opacity,
                                                                   sheetClass.background.opacity.evaluate<float>(z),
                                                                   transitioning.background.opacity,
                                                                   start,
                                                                   sheetClass.background.opacity_transition.duration * 1_millisecond);
            } else if (transitions["background"].count(TransitionablePropertyKey::Opacity)) {
                computed.background.opacity = transitioning.background.opacity;
            } else {
                computed.background.opacity = sheetClass.background.opacity.evaluate<float>(z);
            }
        }
    }

    previouslyAppliedClasses.swap(currentPassAppliedClasses);
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

