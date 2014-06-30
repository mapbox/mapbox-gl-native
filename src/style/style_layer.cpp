#include <llmr/style/style_layer.hpp>
#include <llmr/style/style_bucket.hpp>
#include <llmr/map/source.hpp>
#include <llmr/style/style_layer_group.hpp>
#include <llmr/style/property_fallback.hpp>

namespace llmr {

StyleLayer::StyleLayer(const std::string &id, std::map<ClassID, ClassProperties> &&styles,
                       std::unique_ptr<const RasterizeProperties> &&rasterize)
    : id(id), styles(std::move(styles)), rasterize(std::move(rasterize)) {}

bool StyleLayer::isBackground() const {
    return id == "background";
}

void StyleLayer::setClasses(const std::vector<std::string> &class_names, const timestamp now,
                            const PropertyTransition &defaultTransition) {
    // Stores all keys that we have already added transitions for.
    std::set<PropertyKey> already_applied;

    // Reverse iterate through all class names and apply them last to first.
    for (auto it = class_names.rbegin(); it != class_names.rend(); it++) {
        const std::string &class_name = *it;
        // From here on, we're only dealing with IDs to avoid comparing strings all the time.
        const ClassID class_id = ClassDictionary::Lookup(class_name);
        applyClassProperties(class_id, already_applied, now, defaultTransition);
    }

    // As the last class, apply the default class.
    applyClassProperties(ClassID::Default, already_applied, now, defaultTransition);

    // Make sure that we also transition to the fallback value for keys that aren't changed by
    // any applied classes.
    for (std::pair<const PropertyKey, AppliedClassProperties> &property_pair : appliedStyle) {
        const PropertyKey key = property_pair.first;
        if (already_applied.find(key) != already_applied.end()) {
            // This property has already been set by a previous class, so we don't need to
            // transition to the fallback.
            continue;
        }

        AppliedClassProperties &appliedProperties = property_pair.second;
        // Make sure that we don't do double transitions to the fallback value.
        if (appliedProperties.mostRecent() != ClassID::Fallback) {
            // This property key hasn't been set by a previous class, so we need to add a transition
            // to the fallback value for that key.
            const timestamp begin = now + defaultTransition.delay;
            const timestamp end = begin + defaultTransition.duration;
            const PropertyValue &value = PropertyFallbackValue::Get(key);
            appliedProperties.add(ClassID::Fallback, begin, end, value);
        }
    }

    // Update all child layers as well.
    if (layers) {
        layers->setClasses(class_names, now, defaultTransition);
    }
}

// Helper function for applying all properties of a a single class that haven't been applied yet.
void StyleLayer::applyClassProperties(const ClassID class_id,
                                      std::set<PropertyKey> &already_applied, timestamp now,
                                      const PropertyTransition &defaultTransition) {
    auto style_it = styles.find(class_id);
    if (style_it == styles.end()) {
        // There is no class in this layer with this class_name.
        return;
    }

    // Loop through all the properties in this style, and add transitions to them, if they're
    // not already the most recent transition.
    const ClassProperties &properties = style_it->second;
    for (const std::pair<PropertyKey, PropertyValue> &property_pair : properties)  {
        PropertyKey key = property_pair.first;
        if (already_applied.find(key) != already_applied.end()) {
            // This property has already been set by a previous class.
            continue;
        }

        // Mark this property as written by a previous class, so that subsequent
        // classes won't override this.
        already_applied.insert(key);

        // If the most recent transition is not the one with the highest priority, create
        // a transition.
        AppliedClassProperties &appliedProperties = appliedStyle[key];
        if (appliedProperties.mostRecent() != class_id) {
            const PropertyTransition &transition =
                properties.getTransition(key, defaultTransition);
            const timestamp begin = now + transition.delay;
            const timestamp end = begin + transition.duration;
            const PropertyValue &value = property_pair.second;
            appliedProperties.add(class_id, begin, end, value);
        }
    }
}

//void applyStyleProperties(StyleProperties &style, const ClassProperties &properties, float z) {
//    if (style.is<FillProperties>()) {
//        FillProperties &fill = style.get<FillProperties>();
//        applyStyleProperty(properties, PropertyKey::FillEnabled, fill.enabled, z);
//        applyStyleProperty(properties, PropertyKey::FillAntialias, fill.antialias, z);
//        applyStyleProperty(properties, PropertyKey::FillOpacity, fill.opacity, z);
//        applyStyleProperty(properties, PropertyKey::FillColor, fill.fill_color);
//        applyStyleProperty(properties, PropertyKey::FillOutlineColor, fill.stroke_color);
//        applyStyleProperty(properties, PropertyKey::FillTranslateX, fill.translate[0], z);
//        applyStyleProperty(properties, PropertyKey::FillTranslateY, fill.translate[1], z);
//        applyStyleProperty(properties, PropertyKey::FillTranslateAnchor, fill.translateAnchor);
//        applyStyleProperty(properties, PropertyKey::FillImage, fill.image);
//    } else if (style.is<LineProperties>()) {
//        LineProperties &line = style.get<LineProperties>();
//        applyStyleProperty(properties, PropertyKey::LineEnabled, line.enabled, z);
//        applyStyleProperty(properties, PropertyKey::LineOpacity, line.opacity, z);
//        applyStyleProperty(properties, PropertyKey::LineColor, line.color);
//        applyStyleProperty(properties, PropertyKey::LineTranslateX, line.translate[0], z);
//        applyStyleProperty(properties, PropertyKey::LineTranslateY, line.translate[1], z);
//        applyStyleProperty(properties, PropertyKey::LineTranslateAnchor, line.translateAnchor);
//        applyStyleProperty(properties, PropertyKey::LineWidth, line.width, z);
//        applyStyleProperty(properties, PropertyKey::LineOffset, line.offset, z);
//        applyStyleProperty(properties, PropertyKey::LineBlur, line.blur, z);
//        applyStyleProperty(properties, PropertyKey::LineDashLand, line.dash_array[0], z);
//        applyStyleProperty(properties, PropertyKey::LineDashGap, line.dash_array[1], z);
//        applyStyleProperty(properties, PropertyKey::LineImage, line.image);
//    } else if (style.is<IconProperties>()) {
//        IconProperties &icon = style.get<IconProperties>();
//        applyStyleProperty(properties, PropertyKey::IconEnabled, icon.enabled, z);
//        applyStyleProperty(properties, PropertyKey::IconOpacity, icon.opacity, z);
//        applyStyleProperty(properties, PropertyKey::IconRotate, icon.rotate, z);
//        applyStyleProperty(properties, PropertyKey::IconRotateAnchor, icon.rotate_anchor);
//    } else if (style.is<TextProperties>()) {
//        TextProperties &text = style.get<TextProperties>();
//        applyStyleProperty(properties, PropertyKey::TextEnabled, text.enabled, z);
//        applyStyleProperty(properties, PropertyKey::TextOpacity, text.opacity, z);
//        applyStyleProperty(properties, PropertyKey::TextSize, text.size, z);
//        applyStyleProperty(properties, PropertyKey::TextColor, text.color);
//        applyStyleProperty(properties, PropertyKey::TextHaloColor, text.halo_color);
//        applyStyleProperty(properties, PropertyKey::TextHaloWidth, text.halo_width, z);
//        applyStyleProperty(properties, PropertyKey::TextHaloBlur, text.halo_blur, z);
//    } else if (style.is<CompositeProperties>()) {
//        CompositeProperties &composite = style.get<CompositeProperties>();
//        applyStyleProperty(properties, PropertyKey::CompositeEnabled, composite.enabled, z);
//        applyStyleProperty(properties, PropertyKey::CompositeOpacity, composite.opacity, z);
//    } else if (style.is<RasterProperties>()) {
//        RasterProperties &raster = style.get<RasterProperties>();
//        applyStyleProperty(properties, PropertyKey::RasterEnabled, raster.enabled, z);
//        applyStyleProperty(properties, PropertyKey::RasterOpacity, raster.opacity, z);
//        applyStyleProperty(properties, PropertyKey::RasterSpin, raster.spin, z);
//        applyStyleProperty(properties, PropertyKey::RasterBrightnessLow, raster.brightness[0], z);
//        applyStyleProperty(properties, PropertyKey::RasterBrightnessHigh, raster.brightness[1], z);
//        applyStyleProperty(properties, PropertyKey::RasterSaturation, raster.saturation, z);
//        applyStyleProperty(properties, PropertyKey::RasterContrast, raster.contrast, z);
//        applyStyleProperty(properties, PropertyKey::RasterFade, raster.fade, z);
//    } else if (style.is<BackgroundProperties>()) {
//        BackgroundProperties &background = style.get<BackgroundProperties>();
//        applyStyleProperty(properties, PropertyKey::BackgroundColor, background.color);
//    }
//}
//

template <typename T>
struct PropertyEvaluator {
    typedef T result_type;
    PropertyEvaluator(float z) : z(z) {}

    template <typename P, typename std::enable_if<std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &value) const {
        return value;
    }

    T operator()(const Function<T> &value) const {
        return util::apply_visitor(FunctionEvaluator<T>(z), value);
    }

    template <typename P, typename std::enable_if<!std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &) const {
        return T();
    }

private:
    const float z;
};

template <typename T>
void StyleLayer::applyStyleProperty(PropertyKey key, T &target, const float z, const timestamp t) {
    auto it = appliedStyle.find(key);
    if (it != appliedStyle.end()) {
        AppliedClassProperties &applied = it->second;
        // Iterate through all properties that we need to apply in order.
        const PropertyEvaluator<T> evaluator(z);
        for (AppliedClassProperty &property : applied.properties) {
            if (property.end >= t) {
                // We overwrite the current property with the new value.
                target = util::apply_visitor(evaluator, property.value);
            } else if (property.begin <= t) {
                // We overwrite the current property partially with the new value.
                float progress = float(t - property.begin) / float(property.end - property.begin);
                target = util::apply_visitor(evaluator, property.value);
//                target = (1.0f - progress) * target + (progress) *
//                evaluateProperty(property.value, target, z);
            }
        }
    }
}


template <typename T>
void StyleLayer::applyStyleProperties(float, timestamp) {
    properties.set<std::false_type>();
}

template <>
void StyleLayer::applyStyleProperties<FillProperties>(const float z, const timestamp t) {
    properties.set<FillProperties>();
    FillProperties &fill = properties.get<FillProperties>();
    applyStyleProperty(PropertyKey::FillEnabled, fill.enabled, z, t);
    applyStyleProperty(PropertyKey::FillAntialias, fill.antialias, z, t);
    applyStyleProperty(PropertyKey::FillOpacity, fill.opacity, z, t);
    applyStyleProperty(PropertyKey::FillColor, fill.fill_color, z, t);
    applyStyleProperty(PropertyKey::FillOutlineColor, fill.stroke_color, z, t);
    applyStyleProperty(PropertyKey::FillTranslateX, fill.translate[0], z, t);
    applyStyleProperty(PropertyKey::FillTranslateY, fill.translate[1], z, t);
    applyStyleProperty(PropertyKey::FillTranslateAnchor, fill.translateAnchor, z, t);
    applyStyleProperty(PropertyKey::FillImage, fill.image, z, t);
}

template <>
void StyleLayer::applyStyleProperties<LineProperties>(const float z, const timestamp t) {
    properties.set<LineProperties>();
    LineProperties &line = properties.get<LineProperties>();
    applyStyleProperty(PropertyKey::LineEnabled, line.enabled, z, t);
    applyStyleProperty(PropertyKey::LineOpacity, line.opacity, z, t);
    applyStyleProperty(PropertyKey::LineColor, line.color, z, t);
    applyStyleProperty(PropertyKey::LineTranslateX, line.translate[0], z, t);
    applyStyleProperty(PropertyKey::LineTranslateY, line.translate[1], z, t);
    applyStyleProperty(PropertyKey::LineTranslateAnchor, line.translateAnchor, z, t);
    applyStyleProperty(PropertyKey::LineWidth, line.width, z, t);
    applyStyleProperty(PropertyKey::LineOffset, line.offset, z, t);
    applyStyleProperty(PropertyKey::LineBlur, line.blur, z, t);
    applyStyleProperty(PropertyKey::LineDashLand, line.dash_array[0], z, t);
    applyStyleProperty(PropertyKey::LineDashGap, line.dash_array[1], z, t);
    applyStyleProperty(PropertyKey::LineImage, line.image, z, t);
}

void StyleLayer::updateProperties(float z, const timestamp t) {
    if (layers) {
        layers->updateProperties(z, t);
    }

    // Accomodate for different tile size.
    if (bucket && bucket->source) {
        z += std::log(bucket->source->tile_size / 256.0f) / M_LN2;
    }

    if (layers) {
        applyStyleProperties<CompositeProperties>(z, t);
    } else if (bucket) {
        switch (bucket->type) {
            case BucketType::Fill: applyStyleProperties<FillProperties>(z, t); break;
            case BucketType::Line: applyStyleProperties<LineProperties>(z, t); break;
            case BucketType::Icon: applyStyleProperties<IconProperties>(z, t); break;
            case BucketType::Text: applyStyleProperties<TextProperties>(z, t); break;
            case BucketType::Raster: applyStyleProperties<RasterProperties>(z, t); break;
            default: properties.set<std::false_type>(); break;
        }
    } else {
        return applyStyleProperties<BackgroundProperties>(z, t);
    }
}


}
