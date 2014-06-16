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

void Style::cascadeProperties(GenericProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    using Key = ClassPropertyKey;

    if (const FunctionProperty *enabled = klass.get<FunctionProperty>(Key::Enabled)) {
        properties.enabled = enabled->evaluate<bool>(z);
    }

    if (const FunctionProperty *translateX = klass.get<FunctionProperty>(Key::TranslateX)) {
        properties.translate[0] = translateX->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
    }

    if (const FunctionProperty *translateY = klass.get<FunctionProperty>(Key::TranslateY)) {
        properties.translate[1] = translateY->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Translate] = class_name;
    }

    if (const PropertyTransition *translateTransition = klass.get<PropertyTransition>(Key::TranslateTransition)) {
        if (translateTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Translate, *translateTransition);
        }
    }

    if (const TranslateAnchor *translateAnchor = klass.get<TranslateAnchor>(Key::TranslateAnchor)) {
        properties.translateAnchor = *translateAnchor;
    }

    if (const FunctionProperty *opacity = klass.get<FunctionProperty>(Key::Opacity)) {
        properties.opacity = opacity->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Opacity] = class_name;
    }

    if (const PropertyTransition *opacityTransition = klass.get<PropertyTransition>(Key::OpacityTransition)) {
        if (opacityTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Opacity, *opacityTransition);
        }
    }

    if (const FunctionProperty *prerender = klass.get<FunctionProperty>(Key::Prerender)) {
        properties.prerender = *prerender;
    }

    if (const FunctionProperty *prerenderBuffer = klass.get<FunctionProperty>(Key::PrerenderBuffer)) {
        properties.prerenderBuffer = *prerenderBuffer;
    }

    if (const FunctionProperty *prerenderSize = klass.get<FunctionProperty>(Key::PrerenderSize)) {
        properties.prerenderSize = *prerenderSize;
    }

    if (const FunctionProperty *prerenderBlur = klass.get<FunctionProperty>(Key::PrerenderBlur)) {
        properties.prerenderBlur = *prerenderBlur;
    }
}

void Style::cascadeProperties(FillProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);

    using Key = ClassPropertyKey;

    if (const Winding *winding = klass.get<Winding>(Key::FillWinding)) {
        properties.winding = *winding;
    }

    if (const FunctionProperty *antialias = klass.get<FunctionProperty>(Key::FillAntialias)) {
        properties.antialias = antialias->evaluate<bool>(z);
    }

    if (const Color *fillColor = klass.get<Color>(Key::FillColor)) {
        properties.fill_color = *fillColor;
        computed.effective_classes[layer_name][TransitionablePropertyKey::FillColor] = class_name;
    }

    if (const PropertyTransition *fillColorTransition = klass.get<PropertyTransition>(Key::FillColorTransition)) {
        if (fillColorTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::FillColor, *fillColorTransition);
        }
    }

    if (const Color *fillStrokeColor = klass.get<Color>(Key::FillStrokeColor)) {
        properties.stroke_color = *fillStrokeColor;
        computed.effective_classes[layer_name][TransitionablePropertyKey::StrokeColor] = class_name;
    }

    if (const PropertyTransition *fillStrokeColorTransition = klass.get<PropertyTransition>(Key::FillStrokeColorTransition)) {
        if (fillStrokeColorTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::StrokeColor, *fillStrokeColorTransition);
        }
    }

    if (const std::string *image = klass.get<std::string>(Key::FillImage)) {
        properties.image = *image;
    }
}

void Style::cascadeProperties(LineProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);

    using Key = ClassPropertyKey;

    if (const FunctionProperty *width = klass.get<FunctionProperty>(Key::LineWidth)) {
        properties.width = width->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Width] = class_name;
    }

    if (const PropertyTransition *width_transition = klass.get<PropertyTransition>(Key::LineWidthTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Width, *width_transition);
    }

    if (const FunctionProperty *offset = klass.get<FunctionProperty>(Key::LineOffset)) {
        properties.offset = offset->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Offset] = class_name;
    }

    if (const PropertyTransition *offset_transition = klass.get<PropertyTransition>(Key::LineOffsetTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Offset, *offset_transition);
    }

    if (const Color *color = klass.get<Color>(Key::LineColor)) {
        properties.color = *color;
        computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
    }

    if (const PropertyTransition *color_transition = klass.get<PropertyTransition>(Key::LineColorTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, *color_transition);
    }

    if (const FunctionProperty *dash_array = klass.get<FunctionProperty>(Key::LineDashLand)) {
        properties.dash_array[0] = dash_array->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::DashArray] = class_name;
    }

    if (const FunctionProperty *dash_array = klass.get<FunctionProperty>(Key::LineDashGap)) {
        properties.dash_array[1] = dash_array->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::DashArray] = class_name;
    }

    if (const PropertyTransition *dash_array_transition = klass.get<PropertyTransition>(Key::LineDashTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::DashArray, *dash_array_transition);
    }
}

void Style::cascadeProperties(TextProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);

    using Key = ClassPropertyKey;

    if (const Color *color = klass.get<Color>(Key::TextColor)) {
        properties.color = *color;
        computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
    }

    if (const PropertyTransition *colorTransition = klass.get<PropertyTransition>(Key::TextColorTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, *colorTransition);
    }

    if (const FunctionProperty *size = klass.get<FunctionProperty>(Key::TextSize)) {
        properties.size = size->evaluate<float>(z);
    }

    if (const Color *haloColor = klass.get<Color>(Key::TextHaloColor)) {
        properties.halo = *haloColor;
        computed.effective_classes[layer_name][TransitionablePropertyKey::Halo] = class_name;
    }

    if (const PropertyTransition *haloColorTransition = klass.get<PropertyTransition>(Key::TextHaloColorTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Halo, *haloColorTransition);
    }

    if (const FunctionProperty *haloRadius = klass.get<FunctionProperty>(Key::TextHaloRadius)) {
        properties.halo_radius = haloRadius->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::HaloRadius] = class_name;
    }

    if (const PropertyTransition *haloRadiusTransition = klass.get<PropertyTransition>(Key::TextHaloRadiusTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::HaloRadius, *haloRadiusTransition);
    }

    if (const FunctionProperty *haloBlur = klass.get<FunctionProperty>(Key::TextHaloBlur)) {
        properties.halo_blur = haloBlur->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::HaloBlur] = class_name;
    }

    if (const PropertyTransition *haloBlurTransition = klass.get<PropertyTransition>(Key::TextHaloBlurTransition)) {
        properties_to_transition[layer_name].emplace(TransitionablePropertyKey::HaloBlur, *haloBlurTransition);
    }

    if (const FunctionProperty *rotate = klass.get<FunctionProperty>(Key::TextRotate)) {
        properties.rotate = rotate->evaluate<float>(z);
    }

    if (const FunctionProperty *alwaysVisible = klass.get<FunctionProperty>(Key::TextAlwaysVisible)) {
        properties.always_visible = alwaysVisible->evaluate<bool>(z);
    }
}

void Style::cascadeProperties(IconProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);

    using Key = ClassPropertyKey;

    if (const Color *color = klass.get<Color>(Key::IconColor)) {
        properties.color = *color;
        computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
    }

    if (const PropertyTransition *colorTransition = klass.get<PropertyTransition>(Key::IconColorTransition)) {
        if (colorTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, *colorTransition);
        }
    }

    if (const FunctionProperty *size = klass.get<FunctionProperty>(Key::IconSize)) {
        properties.size = size->evaluate<float>(z);
    }

    if (const std::string *image = klass.get<std::string>(Key::IconImage)) {
        properties.image = *image;
    }

    if (const FunctionProperty *radius = klass.get<FunctionProperty>(Key::IconRadius)) {
        properties.radius = radius->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Radius] = class_name;
    }

    if (const PropertyTransition *radiusTransition = klass.get<PropertyTransition>(Key::IconRadiusTransition)) {
        if (radiusTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Radius, *radiusTransition);
        }
    }

    if (const FunctionProperty *blur = klass.get<FunctionProperty>(Key::IconBlur)) {
        properties.blur = blur->evaluate<float>(z);
        computed.effective_classes[layer_name][TransitionablePropertyKey::Blur] = class_name;
    }

    if (const PropertyTransition *blurTransition = klass.get<PropertyTransition>(Key::IconBlurTransition)) {
        if (blurTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Blur, *blurTransition);
        }
    }
}

void Style::cascadeProperties(RasterProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);
}

void Style::cascadeProperties(CompositeProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);
}

void Style::cascadeProperties(BackgroundProperties &properties, const ClassProperties& klass, const std::string& layer_name, const std::string& class_name, float z) {
    cascadeProperties(reinterpret_cast<GenericProperties &>(properties), klass, layer_name, class_name, z);

    using Key = ClassPropertyKey;

    if (const Color *backgroundColor = klass.get<Color>(Key::BackgroundColor)) {
        properties.color = *backgroundColor;
        computed.effective_classes[layer_name][TransitionablePropertyKey::Color] = class_name;
    }

    if (const PropertyTransition *colorTransition = klass.get<PropertyTransition>(Key::TextColorTransition)) {
        if (colorTransition->duration) {
            properties_to_transition[layer_name].emplace(TransitionablePropertyKey::Color, *colorTransition);
        }
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

        using Key = ClassPropertyKey;

        // Skip if not enabled.
        if (appliedClasses.find(class_name) == appliedClasses.end()) continue;

        for (const auto &pair : sheetClass) {
            const std::string& layer_name = pair.first;
            const ClassProperties& properties = pair.second;

            // Find out what bucket this class is
            switch (properties.type) {
            case RenderType::Fill:
                cascadeProperties(computed.fills[layer_name], properties, layer_name, class_name, z);
                break;
            case RenderType::Line:
                cascadeProperties(computed.lines[layer_name], properties, layer_name, class_name, z);
                break;
            case RenderType::Icon:
                cascadeProperties(computed.icons[layer_name], properties, layer_name, class_name, z);
                break;
            case RenderType::Text:
                cascadeProperties(computed.texts[layer_name], properties, layer_name, class_name, z);
                break;
            case RenderType::Raster:
                cascadeProperties(computed.rasters[layer_name], properties, layer_name, class_name, z);
                break;
            case RenderType::Composite:
                cascadeProperties(computed.composites[layer_name], properties, layer_name, class_name, z);
                break;
            case RenderType::Background:
                cascadeProperties(computed.background, properties, layer_name, class_name, z);
                break;
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

