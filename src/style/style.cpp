#include <llmr/style/style.hpp>
#include <llmr/style/style_layer_group.hpp>
#include <llmr/style/style_parser.hpp>
#include <llmr/style/style_bucket.hpp>
#include <llmr/util/constants.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>


namespace llmr {

Style::Style() {
}

void Style::updateSources() {
    activeSources.clear();
    updateSources(layers);
}

const std::set<std::shared_ptr<Source>> Style::getActiveSources() const {
    return activeSources;
}

void Style::updateSources(const std::shared_ptr<StyleLayerGroup> &group) {
    if (!group) {
        return;
    }
    for (const std::shared_ptr<StyleLayer> &layer : group->layers) {
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
void applyProperty(const ClassProperties &properties, PropertyKey key, T &target) {
    auto it = properties.properties.find(key);
    if (it != properties.properties.end()) {
        const PropertyValue &value = it->second;
        if (value.is<T>()) {
            target = value.get<T>();
        }
    }
}

template <typename T>
void applyProperty(const ClassProperties &properties, PropertyKey key, T &target, float z) {
    auto it = properties.properties.find(key);
    if (it != properties.properties.end()) {
        const PropertyValue &value = it->second;
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
        applyProperty(properties, PropertyKey::FillEnabled, fill.enabled, z);
        applyProperty(properties, PropertyKey::FillAntialias, fill.antialias, z);
        applyProperty(properties, PropertyKey::FillOpacity, fill.opacity, z);
        applyProperty(properties, PropertyKey::FillColor, fill.fill_color);
        applyProperty(properties, PropertyKey::FillOutlineColor, fill.stroke_color);
        applyProperty(properties, PropertyKey::FillTranslateX, fill.translate[0], z);
        applyProperty(properties, PropertyKey::FillTranslateY, fill.translate[1], z);
        applyProperty(properties, PropertyKey::FillTranslateAnchor, fill.translateAnchor);
        applyProperty(properties, PropertyKey::FillImage, fill.image);
    } else if (style.is<LineProperties>()) {
        LineProperties &line = style.get<LineProperties>();
        applyProperty(properties, PropertyKey::LineEnabled, line.enabled, z);
        applyProperty(properties, PropertyKey::LineOpacity, line.opacity, z);
        applyProperty(properties, PropertyKey::LineColor, line.color);
        applyProperty(properties, PropertyKey::LineTranslateX, line.translate[0], z);
        applyProperty(properties, PropertyKey::LineTranslateY, line.translate[1], z);
        applyProperty(properties, PropertyKey::LineTranslateAnchor, line.translateAnchor);
        applyProperty(properties, PropertyKey::LineWidth, line.width, z);
        applyProperty(properties, PropertyKey::LineOffset, line.offset, z);
        applyProperty(properties, PropertyKey::LineBlur, line.blur, z);
        applyProperty(properties, PropertyKey::LineDashLand, line.dash_array[0], z);
        applyProperty(properties, PropertyKey::LineDashGap, line.dash_array[1], z);
        applyProperty(properties, PropertyKey::LineImage, line.image);
    } else if (style.is<IconProperties>()) {
        IconProperties &icon = style.get<IconProperties>();
        applyProperty(properties, PropertyKey::IconEnabled, icon.enabled, z);
        applyProperty(properties, PropertyKey::IconOpacity, icon.opacity, z);
        applyProperty(properties, PropertyKey::IconRotate, icon.rotate, z);
        applyProperty(properties, PropertyKey::IconRotateAnchor, icon.rotate_anchor);
    } else if (style.is<TextProperties>()) {
        TextProperties &text = style.get<TextProperties>();
        applyProperty(properties, PropertyKey::TextEnabled, text.enabled, z);
        applyProperty(properties, PropertyKey::TextOpacity, text.opacity, z);
        applyProperty(properties, PropertyKey::TextSize, text.size, z);
        applyProperty(properties, PropertyKey::TextColor, text.color);
        applyProperty(properties, PropertyKey::TextHaloColor, text.halo_color);
        applyProperty(properties, PropertyKey::TextHaloWidth, text.halo_width, z);
        applyProperty(properties, PropertyKey::TextHaloBlur, text.halo_blur, z);
    } else if (style.is<CompositeProperties>()) {
        CompositeProperties &composite = style.get<CompositeProperties>();
        applyProperty(properties, PropertyKey::CompositeEnabled, composite.enabled, z);
        applyProperty(properties, PropertyKey::CompositeOpacity, composite.opacity, z);
    } else if (style.is<RasterProperties>()) {
        RasterProperties &raster = style.get<RasterProperties>();
        applyProperty(properties, PropertyKey::RasterEnabled, raster.enabled, z);
        applyProperty(properties, PropertyKey::RasterOpacity, raster.opacity, z);
        applyProperty(properties, PropertyKey::RasterSpin, raster.spin, z);
        applyProperty(properties, PropertyKey::RasterBrightnessLow, raster.brightness[0], z);
        applyProperty(properties, PropertyKey::RasterBrightnessHigh, raster.brightness[1], z);
        applyProperty(properties, PropertyKey::RasterSaturation, raster.saturation, z);
        applyProperty(properties, PropertyKey::RasterContrast, raster.contrast, z);
        applyProperty(properties, PropertyKey::RasterFade, raster.fade, z);
    } else if (style.is<BackgroundProperties>()) {
        BackgroundProperties &background = style.get<BackgroundProperties>();
        applyProperty(properties, PropertyKey::BackgroundColor, background.color);
    }
}

void Style::updateProperties(const std::shared_ptr<StyleLayerGroup> &group, float z, timestamp t) {
    if (!group) {
        return;
    }

    for (const std::shared_ptr<StyleLayer> &layer : group->layers) {
        if (!layer) continue;

        if (layer->layers) {
            updateProperties(layer->layers, z, t);
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

void Style::updateProperties(float z, timestamp t) {
    uv::writelock lock(mtx);

    updateSources();

    updateProperties(layers, z, t);

    // Apply transitions after the first time.
    if (!initial_render_complete) {
        initial_render_complete = true;
        return;
    }
}

void Style::setDefaultTransitionDuration(uint16_t duration_milliseconds) {
    defaultTransition.duration = duration_milliseconds;
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

    updateClasses();
}

void Style::updateClasses() {
    if (layers) {
        layers->setClasses(appliedClasses, util::now(), defaultTransition);
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
