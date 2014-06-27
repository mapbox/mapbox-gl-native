#include <llmr/style/style.hpp>
#include <llmr/style/style_parser.hpp>
#include <llmr/util/constants.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>


namespace llmr {

Style::Style() {
}

void Style::reset() {
}

void Style::updateSources() {
    activeSources.clear();
    updateSources(layers);
}

const std::set<std::shared_ptr<Source>> Style::getActiveSources() const {
    return activeSources;
}

void Style::updateSources(const std::shared_ptr<StyleLayerGroup> &layers) {
    if (!layers) {
        return;
    }
    for (const std::shared_ptr<StyleLayer> &layer : *layers) {
        if (!layer) continue;
        if (layer->bucket) {
            if (layer->bucket->source) {
                activeSources.emplace(layer->bucket->source);
            }
        } else if (layer->layers) {
            updateSources(layer->layers);
        }
    }
}

StyleProperties resetClassProperties(StyleLayer &layer) {
    if (layer.layers) {
        return CompositeProperties();
    } else if (layer.bucket) {
        switch (layer.bucket->type) {
            case BucketType::Fill: return FillProperties(); break;
            case BucketType::Line: return LineProperties(); break;
            case BucketType::Icon: return IconProperties(); break;
            case BucketType::Text: return TextProperties(); break;
            case BucketType::Raster: return RasterProperties(); break;
            default: return std::false_type(); break;
        }
    } else {
        return BackgroundProperties();
    }
}

template <typename T>
void applyProperty(const ClassProperties &properties, ClassPropertyKey key, T &target) {
    auto it = properties.properties.find(key);
    if (it != properties.properties.end()) {
        const ClassPropertyValue &value = it->second;
        if (value.is<T>()) {
            target = value.get<T>();
        }
    }
}

template <typename T>
void applyProperty(const ClassProperties &properties, ClassPropertyKey key, T &target, float z) {
    auto it = properties.properties.find(key);
    if (it != properties.properties.end()) {
        const ClassPropertyValue &value = it->second;
        if (value.is<FunctionProperty>()) {
            target = value.get<FunctionProperty>().evaluate<T>(z);
        } else if (value.is<bool>()) {
            target = value.get<bool>();
        }
    }
}

void applyClassProperties(StyleProperties &style, const ClassProperties &properties, float z) {
    if (style.is<FillProperties>()) {
        FillProperties &fill = style.get<FillProperties>();
        applyProperty(properties, ClassPropertyKey::FillEnabled, fill.enabled, z);
        applyProperty(properties, ClassPropertyKey::FillAntialias, fill.antialias, z);
        applyProperty(properties, ClassPropertyKey::FillOpacity, fill.opacity, z);
        applyProperty(properties, ClassPropertyKey::FillColor, fill.fill_color);
        applyProperty(properties, ClassPropertyKey::FillOutlineColor, fill.stroke_color);
        applyProperty(properties, ClassPropertyKey::FillTranslateX, fill.translate[0], z);
        applyProperty(properties, ClassPropertyKey::FillTranslateY, fill.translate[1], z);
        applyProperty(properties, ClassPropertyKey::FillTranslateAnchor, fill.translateAnchor);
        applyProperty(properties, ClassPropertyKey::FillImage, fill.image);
    } else if (style.is<LineProperties>()) {
        LineProperties &line = style.get<LineProperties>();
        applyProperty(properties, ClassPropertyKey::LineEnabled, line.enabled, z);
        applyProperty(properties, ClassPropertyKey::LineOpacity, line.opacity, z);
        applyProperty(properties, ClassPropertyKey::LineColor, line.color);
        applyProperty(properties, ClassPropertyKey::LineTranslateX, line.translate[0], z);
        applyProperty(properties, ClassPropertyKey::LineTranslateY, line.translate[1], z);
        applyProperty(properties, ClassPropertyKey::LineTranslateAnchor, line.translateAnchor);
        applyProperty(properties, ClassPropertyKey::LineWidth, line.width, z);
        applyProperty(properties, ClassPropertyKey::LineOffset, line.offset, z);
        applyProperty(properties, ClassPropertyKey::LineBlur, line.blur, z);
        applyProperty(properties, ClassPropertyKey::LineDashLand, line.dash_array[0], z);
        applyProperty(properties, ClassPropertyKey::LineDashGap, line.dash_array[1], z);
        applyProperty(properties, ClassPropertyKey::LineImage, line.image);
    } else if (style.is<IconProperties>()) {
        IconProperties &icon = style.get<IconProperties>();
        applyProperty(properties, ClassPropertyKey::IconEnabled, icon.enabled, z);
        applyProperty(properties, ClassPropertyKey::IconOpacity, icon.opacity, z);
        applyProperty(properties, ClassPropertyKey::IconRotate, icon.rotate, z);
        applyProperty(properties, ClassPropertyKey::IconRotateAnchor, icon.rotate_anchor);
    } else if (style.is<TextProperties>()) {
        TextProperties &text = style.get<TextProperties>();
        applyProperty(properties, ClassPropertyKey::TextEnabled, text.enabled, z);
        applyProperty(properties, ClassPropertyKey::TextOpacity, text.opacity, z);
        applyProperty(properties, ClassPropertyKey::TextSize, text.size, z);
        applyProperty(properties, ClassPropertyKey::TextColor, text.color);
        applyProperty(properties, ClassPropertyKey::TextHaloColor, text.halo_color);
        applyProperty(properties, ClassPropertyKey::TextHaloWidth, text.halo_width, z);
        applyProperty(properties, ClassPropertyKey::TextHaloBlur, text.halo_blur, z);
    } else if (style.is<CompositeProperties>()) {
        CompositeProperties &composite = style.get<CompositeProperties>();
        applyProperty(properties, ClassPropertyKey::CompositeEnabled, composite.enabled, z);
        applyProperty(properties, ClassPropertyKey::CompositeOpacity, composite.opacity, z);
    } else if (style.is<RasterProperties>()) {
        RasterProperties &raster = style.get<RasterProperties>();
        applyProperty(properties, ClassPropertyKey::RasterSpin, raster.spin, z);
        applyProperty(properties, ClassPropertyKey::RasterBrightnessLow, raster.brightness[0], z);
        applyProperty(properties, ClassPropertyKey::RasterBrightnessHigh, raster.brightness[1], z);
        applyProperty(properties, ClassPropertyKey::RasterSaturation, raster.saturation, z);
        applyProperty(properties, ClassPropertyKey::RasterContrast, raster.contrast, z);
        applyProperty(properties, ClassPropertyKey::RasterFade, raster.fade, z);
    } else if (style.is<BackgroundProperties>()) {
        BackgroundProperties &background = style.get<BackgroundProperties>();
        applyProperty(properties, ClassPropertyKey::BackgroundColor, background.color);
    }
}

void Style::cascade(const std::shared_ptr<StyleLayerGroup> &layers, float z) {
    if (!layers) {
        return;
    }

    for (const std::shared_ptr<StyleLayer> &layer : *layers) {
        if (!layer) continue;

        if (layer->layers) {
            cascade(layer->layers, z);
        }

        // Accomodate for different tile size.
        if (layer->bucket) {
            const StyleBucket &bucket = *layer->bucket;
            if (bucket.source) {
                const Source &source = *bucket.source;
                z += std::log(source.tile_size / 256.0f) / M_LN2;
            }
        }

        layer->style = resetClassProperties(*layer);

        // Apply default class (if exists)
        auto default_it = layer->styles.find(ClassID::Default);
        if (default_it != layer->styles.end()) {
            applyClassProperties(layer->style, default_it->second, z);
        }

        // Apply overriding classes in order.
        for (const std::string &class_name : appliedClasses) {
            const ClassID class_id = ClassDictionary::Lookup(class_name);
            auto class_it = layer->styles.find(class_id);
            if (class_it != layer->styles.end()) {
                applyClassProperties(layer->style, class_it->second, z);
            }
        }
    }
}

void Style::cascade(float z) {
    uv::writelock lock(mtx);

    updateSources();

    reset();
    cascade(layers, z);

    // Apply transitions after the first time.
    if (!initial_render_complete) {
        initial_render_complete = true;
        return;
    }
}


bool Style::needsTransition() const {
    uv::readlock lock(mtx);

    return false;
}

void Style::updateTransitions(timestamp now) {
    uv::writelock lock(mtx);
}

void Style::cancelTransitions() {
    uv::writelock lock(mtx);
}

void Style::setDefaultTransitionDuration(uint64_t duration_milliseconds) {
    default_transition_duration = duration_milliseconds;
}

const std::vector<std::string> &Style::getAppliedClasses() const {
    return appliedClasses;
}

void Style::toggleClass(const std::string &name) {
    if (name.length()) {
        auto it = std::find(appliedClasses.begin(), appliedClasses.end(), name);
        if (it == appliedClasses.end()) {
            appliedClasses.push_back(name);
        } else {
            appliedClasses.erase(it);
        }
    }
}

void Style::loadJSON(const uint8_t *const data) {
    uv::writelock lock(mtx);

    rapidjson::Document doc;
    doc.Parse<0>((const char *const)data);

    StyleParser parser;
    parser.parse(const_cast<const rapidjson::Document &>(doc));

    layers = parser.getLayers();
}

}
