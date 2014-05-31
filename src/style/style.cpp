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
    computed.composites.clear();
    computed.background.color = {{ 1, 1, 1, 1 }};
    computed.background.opacity = 1.0;

    properties_to_transition.clear();
}

void Style::cascadeProperties(GenericProperties &properties, const GenericClass& klass, const std::string& layer_name, const std::string& class_name, float z) {
    if (klass.enabled) {
        properties.enabled = klass.enabled.get().evaluate<bool>(z);
    }

    if (klass.translate) {
        properties.translate = {{ klass.translate.get()[0].evaluate<float>(z),
                            klass.translate.get()[1].evaluate<float>(z) }};
        computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
        if (klass.translate_transition && klass.translate_transition.get().duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Translate, klass.translate_transition.get());
        }
    }

    if (klass.translateAnchor) {
        properties.translateAnchor = klass.translateAnchor.get();
    }

    if (klass.opacity) {
        properties.opacity = klass.opacity.get().evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
        if (klass.opacity_transition && klass.opacity_transition.get().duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Opacity, klass.opacity_transition.get());
        }
    }

    if (klass.prerender) {
        properties.prerender = klass.prerender.get();
    }

    if (klass.prerenderBuffer) {
        properties.prerenderBuffer = klass.prerenderBuffer.get();
    }

    if (klass.prerenderSize) {
        properties.prerenderSize = klass.prerenderSize.get();
    }

    if (klass.prerenderBlur) {
        properties.prerenderBlur = klass.prerenderBlur.get();
    }
}


void Style::cascade(float z) {
    uv::writelock lock(mtx);

    time start = util::now();

    previous.fills.swap(computed.fills);
    previous.lines.swap(computed.lines);
    previous.icons.swap(computed.icons);
    previous.texts.swap(computed.texts);
    previous.rasters.swap(computed.rasters);
    previous.composites.swap(computed.composites);
    previous.background = computed.background;

    previous.effective_classes.swap(computed.effective_classes);

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

            cascadeProperties(fill, layer, layer_name, class_name, z);

            if (layer.winding) {
                fill.winding = layer.winding.get();
            }

            if (layer.antialias) {
                fill.antialias = layer.antialias.get().evaluate<bool>(z);
            }

            if (layer.fill_color) {
                fill.fill_color = layer.fill_color.get();
                computed.effective_classes[layer_name][TransitionablePropertyKey::FillColor] = class_name;
                if (layer.fill_color_transition && layer.fill_color_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::FillColor, layer.fill_color_transition.get());
                }
            }

            if (layer.stroke_color) {
                fill.stroke_color = layer.stroke_color.get();
                computed.effective_classes[layer_name][TransitionablePropertyKey::StrokeColor] = class_name;
                if (layer.stroke_color_transition && layer.stroke_color_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::StrokeColor, layer.stroke_color_transition.get());
                }
            }

            if (layer.image) {
                fill.image = layer.image.get();
            }
        }

        // Cascade line classes.
        for (const auto& line_pair : sheetClass.line) {
            const std::string& layer_name = line_pair.first;
            const llmr::LineClass& layer = line_pair.second;

            llmr::LineProperties& stroke = computed.lines[layer_name];

            cascadeProperties(stroke, layer, layer_name, class_name, z);

            if (layer.width) {
                stroke.width = layer.width.get().evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Width] = class_name;
                if (layer.width_transition && layer.width_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Width, layer.width_transition.get());
                }

            }

            if (layer.offset) {
                stroke.offset = layer.offset.get().evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Offset] = class_name;
                if (layer.offset_transition && layer.offset_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Offset, layer.offset_transition.get());
                }
            }

            if (layer.color) {
                stroke.color = layer.color.get();
                computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
                if (layer.color_transition && layer.color_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, layer.color_transition.get());
                }
            }

            if (layer.dash_array) {
                stroke.dash_array = {{ layer.dash_array.get()[0].evaluate<float>(z),
                                       layer.dash_array.get()[1].evaluate<float>(z) }};
                computed.effective_classes[layer_name][TransitionablePropertyKey::DashArray] = class_name;
                if (layer.dash_array_transition && layer.dash_array_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::DashArray, layer.dash_array_transition.get());
                }
            }
        }

        // Cascade icon classes.
        for (const auto& icon_pair : sheetClass.icon) {
            const std::string& layer_name = icon_pair.first;
            const llmr::IconClass& layer = icon_pair.second;

            llmr::IconProperties& icon = computed.icons[layer_name];

            cascadeProperties(icon, layer, layer_name, class_name, z);

            if (layer.color) {
                icon.color = layer.color.get();
                computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
                if (layer.color_transition && layer.color_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, layer.color_transition.get());
                }
            }

            if (layer.size) {
                icon.size = layer.size.get().evaluate<float>(z);
            }

            if (layer.image) {
                icon.image = layer.image.get();
            }

            if (layer.radius) {
                icon.radius = layer.radius.get().evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Radius] = class_name;
                if (layer.radius_transition && layer.radius_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Radius, layer.radius_transition.get());
                }
            }

            if (layer.blur) {
                icon.blur = layer.blur.get().evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::Blur] = class_name;
                if (layer.blur_transition && layer.blur_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Blur, layer.blur_transition.get());
                }
            }
        }

        // Cascade text classes.
        for (const auto& text_pair : sheetClass.text) {
            const std::string& layer_name = text_pair.first;
            const llmr::TextClass& layer = text_pair.second;

            llmr::TextProperties& text = computed.texts[layer_name];

            cascadeProperties(text, layer, layer_name, class_name, z);

            if (layer.color) {
                text.color = layer.color.get();
                computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
                if (layer.color_transition && layer.color_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, layer.color_transition.get());
                }
            }

            if (layer.size) {
                text.size = layer.size.get().evaluate<float>(z);
            }

            if (layer.halo) {
                text.halo = layer.halo.get();
                computed.effective_classes[layer_name][TransitionablePropertyKey::Halo] = class_name;
                if (layer.halo_transition && layer.halo_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Halo, layer.halo_transition.get());
                }
            }

            if (layer.halo_radius) {
                text.halo_radius = layer.halo_radius.get().evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::HaloRadius] = class_name;
                if (layer.halo_radius_transition && layer.halo_radius_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::HaloRadius, layer.halo_radius_transition.get());
                }
            }

            if (layer.halo_blur) {
                text.halo_blur = layer.halo_blur.get().evaluate<float>(z);
                computed.effective_classes[layer_name][TransitionablePropertyKey::HaloBlur] = class_name;
                if (layer.halo_blur_transition && layer.halo_blur_transition.get().duration) {
                    properties_to_transition[layer_name].emplace(TransitionablePropertyKey::HaloBlur, layer.halo_blur_transition.get());
                }
            }

            if (layer.rotate) {
                text.rotate = layer.rotate.get().evaluate<float>(z);
            }

            if (layer.always_visible) {
                text.always_visible = layer.always_visible.get().evaluate<bool>(z);
            }
        }

        // Cascade raster classes.
        for (const auto& raster_pair : sheetClass.raster) {
            const std::string& layer_name = raster_pair.first;
            const llmr::RasterClass& layer = raster_pair.second;

            llmr::RasterProperties& raster = computed.rasters[layer_name];

            cascadeProperties(raster, layer, layer_name, class_name, z);
        }

        // Cascade composite classes.
        for (const auto& composite_pair : sheetClass.composite) {
            const std::string& layer_name = composite_pair.first;
            const llmr::CompositeClass& layer = composite_pair.second;

            llmr::CompositeProperties& composite = computed.composites[layer_name];

            cascadeProperties(composite, layer, layer_name, class_name, z);
        }

        // Cascade background.
        {
            cascadeProperties(computed.background, sheetClass.background, "background", class_name, z);

            if (sheetClass.background.color) {
                computed.background.color = sheetClass.background.color.get();
                computed.effective_classes["background"][TransitionablePropertyKey::Color] = class_name;
                if (sheetClass.background.color_transition && sheetClass.background.color_transition.get().duration) {
                    properties_to_transition["background"].emplace(TransitionablePropertyKey::Color, sheetClass.background.color_transition.get());
                }
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
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Translate, start)) {

            computed.fills[layer_name].translate = transitioning.fills[layer_name].translate;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Translate)) {

            transitioning.fills[layer_name].translate = previous.fills[layer_name].translate;

            transitions[layer_name][TransitionablePropertyKey::Translate] =
                std::make_shared<util::ease_transition<std::array<float, 2>>>(previous.fills[layer_name].translate,
                                                                              computed.fills[layer_name].translate,
                                                                              transitioning.fills[layer_name].translate,
                                                                              start + transitionDelay(layer_name, TransitionablePropertyKey::Translate),
                                                                              transitionDuration(layer_name, TransitionablePropertyKey::Translate));

            computed.fills[layer_name].translate = transitioning.fills[layer_name].translate;
        }

        // fill color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::FillColor, start)) {

            computed.fills[layer_name].fill_color = transitioning.fills[layer_name].fill_color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::FillColor)) {

            transitioning.fills[layer_name].fill_color = previous.fills[layer_name].fill_color;

            transitions[layer_name][TransitionablePropertyKey::FillColor] =
                std::make_shared<util::ease_transition<Color>>(previous.fills[layer_name].fill_color,
                                                               computed.fills[layer_name].fill_color,
                                                               transitioning.fills[layer_name].fill_color,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::FillColor),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::FillColor));

            computed.fills[layer_name].fill_color = transitioning.fills[layer_name].fill_color;
        }

        // stroke color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::StrokeColor, start)) {

            computed.fills[layer_name].stroke_color = transitioning.fills[layer_name].stroke_color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::StrokeColor)) {

            transitioning.fills[layer_name].stroke_color = previous.fills[layer_name].stroke_color;

            transitions[layer_name][TransitionablePropertyKey::StrokeColor] =
                std::make_shared<util::ease_transition<Color>>(previous.fills[layer_name].stroke_color,
                                                               computed.fills[layer_name].stroke_color,
                                                               transitioning.fills[layer_name].stroke_color,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::StrokeColor),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::StrokeColor));

            computed.fills[layer_name].stroke_color = transitioning.fills[layer_name].stroke_color;
        }

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity, start)) {

            computed.fills[layer_name].opacity = transitioning.fills[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.fills[layer_name].opacity = previous.fills[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.fills[layer_name].opacity,
                                                               computed.fills[layer_name].opacity,
                                                               transitioning.fills[layer_name].opacity,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Opacity),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.fills[layer_name].opacity = transitioning.fills[layer_name].opacity;
        }
    }

    // Lines
    for (const auto& line_pair : computed.lines) {
        const std::string& layer_name = line_pair.first;

        // translate
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Translate, start)) {

            computed.lines[layer_name].translate = transitioning.lines[layer_name].translate;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Translate)) {

            transitioning.lines[layer_name].translate = previous.lines[layer_name].translate;

            transitions[layer_name][TransitionablePropertyKey::Translate] =
                std::make_shared<util::ease_transition<std::array<float, 2>>>(previous.lines[layer_name].translate,
                                                                              computed.lines[layer_name].translate,
                                                                              transitioning.lines[layer_name].translate,
                                                                              start + transitionDelay(layer_name, TransitionablePropertyKey::Translate),
                                                                              transitionDuration(layer_name, TransitionablePropertyKey::Translate));

            computed.lines[layer_name].translate = transitioning.lines[layer_name].translate;
        }

        // width
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Width, start)) {

            computed.lines[layer_name].width = transitioning.lines[layer_name].width;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Width)) {

            transitioning.lines[layer_name].width = previous.lines[layer_name].width;

            transitions[layer_name][TransitionablePropertyKey::Width] =
                std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].width,
                                                               computed.lines[layer_name].width,
                                                               transitioning.lines[layer_name].width,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Width),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Width));

            computed.lines[layer_name].width = transitioning.lines[layer_name].width;
        }

        // offset
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Offset, start)) {

            computed.lines[layer_name].offset = transitioning.lines[layer_name].offset;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Offset)) {

            transitioning.lines[layer_name].offset = previous.lines[layer_name].offset;

            transitions[layer_name][TransitionablePropertyKey::Offset] =
                std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].offset,
                                                               computed.lines[layer_name].offset,
                                                               transitioning.lines[layer_name].offset,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Offset),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Offset));

            computed.lines[layer_name].offset = transitioning.lines[layer_name].offset;
        }

        // color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Color, start)) {

            computed.lines[layer_name].color = transitioning.lines[layer_name].color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Color)) {

            transitioning.lines[layer_name].color = previous.lines[layer_name].color;

            transitions[layer_name][TransitionablePropertyKey::Color] =
                std::make_shared<util::ease_transition<Color>>(previous.lines[layer_name].color,
                                                               computed.lines[layer_name].color,
                                                               transitioning.lines[layer_name].color,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Color),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Color));

            computed.lines[layer_name].color = transitioning.lines[layer_name].color;
        }

        // dasharray
        if (transitionInProgress(layer_name, TransitionablePropertyKey::DashArray, start)) {

            computed.lines[layer_name].dash_array = transitioning.lines[layer_name].dash_array;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::DashArray)) {

            transitioning.lines[layer_name].dash_array = previous.lines[layer_name].dash_array;

            transitions[layer_name][TransitionablePropertyKey::DashArray] =
                std::make_shared<util::ease_transition<std::array<float, 2>>>(previous.lines[layer_name].dash_array,
                                                                              computed.lines[layer_name].dash_array,
                                                                              transitioning.lines[layer_name].dash_array,
                                                                              start + transitionDelay(layer_name, TransitionablePropertyKey::DashArray),
                                                                              transitionDuration(layer_name, TransitionablePropertyKey::DashArray));

            computed.lines[layer_name].dash_array = transitioning.lines[layer_name].dash_array;
        }

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity, start)) {

            computed.lines[layer_name].opacity = transitioning.lines[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.lines[layer_name].opacity = previous.lines[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.lines[layer_name].opacity,
                                                               computed.lines[layer_name].opacity,
                                                               transitioning.lines[layer_name].opacity,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Opacity),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.lines[layer_name].opacity = transitioning.lines[layer_name].opacity;
        }
    }

    // Icons
    for (const auto& icon_pair : computed.icons) {
        const std::string& layer_name = icon_pair.first;

        // translate
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Translate, start)) {

            computed.icons[layer_name].translate = transitioning.icons[layer_name].translate;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Translate)) {

            transitioning.icons[layer_name].translate = previous.icons[layer_name].translate;

            transitions[layer_name][TransitionablePropertyKey::Translate] =
                std::make_shared<util::ease_transition<std::array<float, 2>>>(previous.icons[layer_name].translate,
                                                                              computed.icons[layer_name].translate,
                                                                              transitioning.icons[layer_name].translate,
                                                                              start + transitionDelay(layer_name, TransitionablePropertyKey::Translate),
                                                                              transitionDuration(layer_name, TransitionablePropertyKey::Translate));

            computed.icons[layer_name].translate = transitioning.icons[layer_name].translate;
        }

        // color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Color, start)) {

            computed.icons[layer_name].color = transitioning.icons[layer_name].color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Color)) {

            transitioning.icons[layer_name].color = previous.icons[layer_name].color;

            transitions[layer_name][TransitionablePropertyKey::Color] =
                std::make_shared<util::ease_transition<Color>>(previous.icons[layer_name].color,
                                                               computed.icons[layer_name].color,
                                                               transitioning.icons[layer_name].color,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Color),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Color));

            computed.icons[layer_name].color = transitioning.icons[layer_name].color;
        }

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity, start)) {

            computed.icons[layer_name].opacity = transitioning.icons[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.icons[layer_name].opacity = previous.icons[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.icons[layer_name].opacity,
                                                               computed.icons[layer_name].opacity,
                                                               transitioning.icons[layer_name].opacity,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Opacity),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.icons[layer_name].opacity = transitioning.icons[layer_name].opacity;
        }

        // radius
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Radius, start)) {

            computed.icons[layer_name].radius = transitioning.icons[layer_name].radius;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Radius)) {

            transitioning.icons[layer_name].radius = previous.icons[layer_name].radius;

            transitions[layer_name][TransitionablePropertyKey::Radius] =
                std::make_shared<util::ease_transition<float>>(previous.icons[layer_name].radius,
                                                               computed.icons[layer_name].radius,
                                                               transitioning.icons[layer_name].radius,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Radius),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Radius));

            computed.icons[layer_name].radius = transitioning.icons[layer_name].radius;
        }

        // blur
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Blur, start)) {

            computed.icons[layer_name].blur = transitioning.icons[layer_name].blur;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Blur)) {

            transitioning.icons[layer_name].blur = previous.icons[layer_name].blur;

            transitions[layer_name][TransitionablePropertyKey::Blur] =
                std::make_shared<util::ease_transition<float>>(previous.icons[layer_name].blur,
                                                               computed.icons[layer_name].blur,
                                                               transitioning.icons[layer_name].blur,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Blur),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Blur));

            computed.icons[layer_name].blur = transitioning.icons[layer_name].blur;
        }
    }

    // Text
    for (const auto& text_pair : computed.texts) {
        const std::string& layer_name = text_pair.first;

        // translate
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Translate, start)) {

            computed.texts[layer_name].translate = transitioning.texts[layer_name].translate;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Translate)) {

            transitioning.texts[layer_name].translate = previous.texts[layer_name].translate;

            transitions[layer_name][TransitionablePropertyKey::Translate] =
                std::make_shared<util::ease_transition<std::array<float, 2>>>(previous.texts[layer_name].translate,
                                                                              computed.texts[layer_name].translate,
                                                                              transitioning.texts[layer_name].translate,
                                                                              start + transitionDelay(layer_name, TransitionablePropertyKey::Translate),
                                                                              transitionDuration(layer_name, TransitionablePropertyKey::Translate));

            computed.texts[layer_name].translate = transitioning.texts[layer_name].translate;
        }

        // color
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Color, start)) {

            computed.texts[layer_name].color = transitioning.texts[layer_name].color;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Color)) {

            transitioning.texts[layer_name].color = previous.texts[layer_name].color;

            transitions[layer_name][TransitionablePropertyKey::Color] =
                std::make_shared<util::ease_transition<Color>>(previous.texts[layer_name].color,
                                                               computed.texts[layer_name].color,
                                                               transitioning.texts[layer_name].color,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Color),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Color));

            computed.texts[layer_name].color = transitioning.texts[layer_name].color;
        }

        // halo
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Halo, start)) {

            computed.texts[layer_name].halo = transitioning.texts[layer_name].halo;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Halo)) {

            transitioning.texts[layer_name].halo = previous.texts[layer_name].halo;

            transitions[layer_name][TransitionablePropertyKey::Halo] =
                std::make_shared<util::ease_transition<Color>>(previous.texts[layer_name].halo,
                                                               computed.texts[layer_name].halo,
                                                               transitioning.texts[layer_name].halo,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Halo),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Halo));

            computed.texts[layer_name].halo = transitioning.texts[layer_name].halo;
        }

        // halo radius
        if (transitionInProgress(layer_name, TransitionablePropertyKey::HaloRadius, start)) {

            computed.texts[layer_name].halo_radius = transitioning.texts[layer_name].halo_radius;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::HaloRadius)) {

            transitioning.texts[layer_name].halo_radius = previous.texts[layer_name].halo_radius;

            transitions[layer_name][TransitionablePropertyKey::HaloRadius] =
                std::make_shared<util::ease_transition<float>>(previous.texts[layer_name].halo_radius,
                                                               computed.texts[layer_name].halo_radius,
                                                               transitioning.texts[layer_name].halo_radius,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::HaloRadius),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::HaloRadius));

            computed.texts[layer_name].halo_radius = transitioning.texts[layer_name].halo_radius;
        }

        // halo blur
        if (transitionInProgress(layer_name, TransitionablePropertyKey::HaloBlur, start)) {

            computed.texts[layer_name].halo_blur = transitioning.texts[layer_name].halo_blur;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::HaloBlur)) {

            transitioning.texts[layer_name].halo_blur = previous.texts[layer_name].halo_blur;

            transitions[layer_name][TransitionablePropertyKey::HaloBlur] =
                std::make_shared<util::ease_transition<float>>(previous.texts[layer_name].halo_blur,
                                                               computed.texts[layer_name].halo_blur,
                                                               transitioning.texts[layer_name].halo_blur,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::HaloBlur),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::HaloBlur));

            computed.texts[layer_name].halo_blur = transitioning.texts[layer_name].halo_blur;
        }

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity, start)) {

            computed.texts[layer_name].opacity = transitioning.texts[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.texts[layer_name].opacity = previous.texts[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.texts[layer_name].opacity,
                                                               computed.texts[layer_name].opacity,
                                                               transitioning.texts[layer_name].opacity,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Opacity),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.texts[layer_name].opacity = transitioning.texts[layer_name].opacity;
        }
    }

    // Rasters
    for (const auto& raster_pair : computed.rasters) {
        const std::string& layer_name = raster_pair.first;

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity, start)) {

            computed.rasters[layer_name].opacity = transitioning.rasters[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.rasters[layer_name].opacity = previous.rasters[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.rasters[layer_name].opacity,
                                                               computed.rasters[layer_name].opacity,
                                                               transitioning.rasters[layer_name].opacity,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Opacity),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.rasters[layer_name].opacity = transitioning.rasters[layer_name].opacity;
        }
    }

    // Composites
    for (const auto& composite_pair : computed.composites) {
        const std::string& layer_name = composite_pair.first;

        // opacity
        if (transitionInProgress(layer_name, TransitionablePropertyKey::Opacity, start)) {

            computed.composites[layer_name].opacity = transitioning.composites[layer_name].opacity;

        } else if (inNeedOfTransition(layer_name, TransitionablePropertyKey::Opacity)) {

            transitioning.composites[layer_name].opacity = previous.composites[layer_name].opacity;

            transitions[layer_name][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.composites[layer_name].opacity,
                                                               computed.composites[layer_name].opacity,
                                                               transitioning.composites[layer_name].opacity,
                                                               start + transitionDelay(layer_name, TransitionablePropertyKey::Opacity),
                                                               transitionDuration(layer_name, TransitionablePropertyKey::Opacity));

            computed.composites[layer_name].opacity = transitioning.composites[layer_name].opacity;
        }
    }

    // Background
    {
        // color
        if (transitionInProgress("background", TransitionablePropertyKey::Color, start)) {

            computed.background.color = transitioning.background.color;

        } else if (inNeedOfTransition("background", TransitionablePropertyKey::Color)) {

            transitioning.background.color = previous.background.color;

            transitions["background"][TransitionablePropertyKey::Color] =
                std::make_shared<util::ease_transition<Color>>(previous.background.color,
                                                               computed.background.color,
                                                               transitioning.background.color,
                                                               start + transitionDelay("background", TransitionablePropertyKey::Color),
                                                               transitionDuration("background", TransitionablePropertyKey::Color));

            computed.background.color = transitioning.background.color;
        }

        // opacity
        if (transitionInProgress("background", TransitionablePropertyKey::Opacity, start)) {

            computed.background.opacity = transitioning.background.opacity;

        } else if (inNeedOfTransition("background", TransitionablePropertyKey::Opacity)) {

            transitioning.background.opacity = previous.background.opacity;

            transitions["background"][TransitionablePropertyKey::Opacity] =
                std::make_shared<util::ease_transition<float>>(previous.background.opacity,
                                                               computed.background.opacity,
                                                               transitioning.background.opacity,
                                                               start + transitionDelay("background", TransitionablePropertyKey::Opacity),
                                                               transitionDuration("background", TransitionablePropertyKey::Opacity));

            computed.background.opacity = transitioning.background.opacity;
        }
    }
}

bool Style::transitionInProgress(const std::string &layer_name, TransitionablePropertyKey key,
                                 time start) {
    const auto it = transitions.find(layer_name);
    if (it == transitions.end()) {
        return false;
    }

    const auto &const_transition = it->second;
    const auto const_transition_it = const_transition.find(key);

    if (const_transition_it == const_transition.end()) {
        return false;
    }

    return const_transition_it->second->update(start) != util::transition::complete;
}

bool Style::transitionExists(const std::string &layer_name, TransitionablePropertyKey key) const {
    const auto it = transitions.find(layer_name);
    if (it == transitions.end()) {
        return false;
    }

    const auto &const_transition = it->second;
    return const_transition.find(key) != const_transition.end();
}

static const std::string empty_class_name;
inline const std::string &
find_effective_class(const std::unordered_map<
                         std::string, std::unordered_map<TransitionablePropertyKey, std::string>> &
                         effective_class_keys,
                     const std::string &layer_name, TransitionablePropertyKey key) {
    const auto effective_class_keys_it = effective_class_keys.find(layer_name);
    if (effective_class_keys_it != effective_class_keys.end()) {
        const auto &effective_classes = effective_class_keys_it->second;
        const auto effective_class_it = effective_classes.find(key);
        if (effective_class_it != effective_classes.end()) {
            return effective_class_it->second;
        }
    }

    return empty_class_name;
}

bool Style::inNeedOfTransition(const std::string &layer_name, TransitionablePropertyKey key) const {
    if (!transitionDuration(layer_name, key)) {
        return false;
    } else if (transitionExists(layer_name, key)) {
        return false;
    }

    // This is a const version of this code:
    //    return (computed.effective_classes[layer_name][key] !=
    //            previous.effective_classes[layer_name][key]);

    const std::string &computed_class =
        find_effective_class(computed.effective_classes, layer_name, key);
    const std::string &previous_class =
        find_effective_class(previous.effective_classes, layer_name, key);

    return computed_class != previous_class;
}

uint64_t Style::transitionDuration(const std::string &layer_name,
                                   TransitionablePropertyKey key) const {
    const auto it = properties_to_transition.find(layer_name);
    if (it == properties_to_transition.end()) {
        return default_transition_duration * 1_millisecond;
    }

    const auto &const_properties_to_transition = it->second;
    const auto it2 = const_properties_to_transition.find(key);
    if (it2 != const_properties_to_transition.end()) {
        return it2->second.duration * 1_millisecond;
    } else {
        return default_transition_duration * 1_millisecond;
    }
}

uint64_t Style::transitionDelay(const std::string &layer_name,
                                TransitionablePropertyKey key) const {
    const auto it = properties_to_transition.find(layer_name);
    if (it == properties_to_transition.end()) {
        return 0;
    }

    const auto &const_properties_to_transition = it->second;
    const auto it2 = const_properties_to_transition.find(key);
    if (it2 != const_properties_to_transition.end()) {
        return it2->second.delay * 1_millisecond;
    } else {
        return 0;
    }
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

void Style::setDefaultTransitionDuration(uint64_t duration_milliseconds) {
    default_transition_duration = duration_milliseconds;
}

void Style::loadJSON(const uint8_t *const data) {
    uv::writelock lock(mtx);

    rapidjson::Document doc;

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

