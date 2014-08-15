#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/style/property_fallback.hpp>

namespace mbgl {

StyleLayer::StyleLayer(const std::string &id, std::map<ClassID, ClassProperties> &&styles)
    : id(id), styles(std::move(styles)) {}

bool StyleLayer::isBackground() const {
    return type == StyleLayerType::Background;
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
            const timestamp begin = now + defaultTransition.delay * 1_millisecond;
            const timestamp end = begin + defaultTransition.duration * 1_millisecond;
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
            const timestamp begin = now + transition.delay * 1_millisecond;
            const timestamp end = begin + transition.duration * 1_millisecond;
            const PropertyValue &value = property_pair.second;
            appliedProperties.add(class_id, begin, end, value);
        }
    }
}

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

inline float interpolate(const float a, const float b, const float t) {
    return (1.0f - t) * a + t * b;
}

inline Color interpolate(const Color &a, const Color &b, const float t) {
    const float rt = 1.0f - t;
    return Color {{
        rt * a[0] + t * b[0],
        rt * a[1] + t * b[1],
        rt * a[2] + t * b[2],
        rt * a[3] + t * b[3]
    }};
}

template <typename T>
inline T interpolate(const T a, const T b, const float t) {
    return t >= 0.5 ? b : a;
}

template <typename T>
void StyleLayer::applyStyleProperty(PropertyKey key, T &target, const float z, const timestamp now) {
    auto it = appliedStyle.find(key);
    if (it != appliedStyle.end()) {
        AppliedClassProperties &applied = it->second;
        // Iterate through all properties that we need to apply in order.
        const PropertyEvaluator<T> evaluator(z);
        for (AppliedClassProperty &property : applied.properties) {
            if (now >= property.end) {
                // We overwrite the current property with the new value.
                target = util::apply_visitor(evaluator, property.value);
            } else if (now >= property.begin) {
                // We overwrite the current property partially with the new value.
                float progress = float(now - property.begin) / float(property.end - property.begin);
                target = interpolate(target, util::apply_visitor(evaluator, property.value), progress);
            } else {
                // Do not apply this property because its transition hasn't begun yet.
            }
        }
    }
}

template <>
void StyleLayer::applyStyleProperties<FillProperties>(const float z, const timestamp now) {
    properties.set<FillProperties>();
    FillProperties &fill = properties.get<FillProperties>();
    applyStyleProperty(PropertyKey::FillAntialias, fill.antialias, z, now);
    applyStyleProperty(PropertyKey::FillOpacity, fill.opacity, z, now);
    applyStyleProperty(PropertyKey::FillColor, fill.fill_color, z, now);
    applyStyleProperty(PropertyKey::FillOutlineColor, fill.stroke_color, z, now);
    applyStyleProperty(PropertyKey::FillTranslateX, fill.translate[0], z, now);
    applyStyleProperty(PropertyKey::FillTranslateY, fill.translate[1], z, now);
    applyStyleProperty(PropertyKey::FillTranslateAnchor, fill.translateAnchor, z, now);
    applyStyleProperty(PropertyKey::FillImage, fill.image, z, now);
}

template <>
void StyleLayer::applyStyleProperties<LineProperties>(const float z, const timestamp now) {
    properties.set<LineProperties>();
    LineProperties &line = properties.get<LineProperties>();
    applyStyleProperty(PropertyKey::LineOpacity, line.opacity, z, now);
    applyStyleProperty(PropertyKey::LineColor, line.color, z, now);
    applyStyleProperty(PropertyKey::LineTranslateX, line.translate[0], z, now);
    applyStyleProperty(PropertyKey::LineTranslateY, line.translate[1], z, now);
    applyStyleProperty(PropertyKey::LineTranslateAnchor, line.translateAnchor, z, now);
    applyStyleProperty(PropertyKey::LineWidth, line.width, z, now);
    applyStyleProperty(PropertyKey::LineOffset, line.offset, z, now);
    applyStyleProperty(PropertyKey::LineBlur, line.blur, z, now);
    applyStyleProperty(PropertyKey::LineDashLand, line.dash_array[0], z, now);
    applyStyleProperty(PropertyKey::LineDashGap, line.dash_array[1], z, now);
    applyStyleProperty(PropertyKey::LineImage, line.image, z, now);
}

template <>
void StyleLayer::applyStyleProperties<SymbolProperties>(const float z, const timestamp now) {
    properties.set<SymbolProperties>();
    SymbolProperties &symbol = properties.get<SymbolProperties>();
    applyStyleProperty(PropertyKey::IconOpacity, symbol.icon.opacity, z, now);
    applyStyleProperty(PropertyKey::IconRotate, symbol.icon.rotate, z, now);
    applyStyleProperty(PropertyKey::IconSize, symbol.icon.size, z, now);
    applyStyleProperty(PropertyKey::IconColor, symbol.icon.color, z, now);
    applyStyleProperty(PropertyKey::IconHaloColor, symbol.icon.halo_color, z, now);
    applyStyleProperty(PropertyKey::IconHaloWidth, symbol.icon.halo_width, z, now);
    applyStyleProperty(PropertyKey::IconHaloBlur, symbol.icon.halo_blur, z, now);
    applyStyleProperty(PropertyKey::IconTranslateX, symbol.icon.translate[0], z, now);
    applyStyleProperty(PropertyKey::IconTranslateY, symbol.icon.translate[1], z, now);
    applyStyleProperty(PropertyKey::IconTranslateAnchor, symbol.icon.translate_anchor, z, now);

    applyStyleProperty(PropertyKey::TextOpacity, symbol.text.opacity, z, now);
    applyStyleProperty(PropertyKey::TextSize, symbol.text.size, z, now);
    applyStyleProperty(PropertyKey::TextColor, symbol.text.color, z, now);
    applyStyleProperty(PropertyKey::TextHaloColor, symbol.text.halo_color, z, now);
    applyStyleProperty(PropertyKey::TextHaloWidth, symbol.text.halo_width, z, now);
    applyStyleProperty(PropertyKey::TextHaloBlur, symbol.text.halo_blur, z, now);
    applyStyleProperty(PropertyKey::TextTranslateX, symbol.text.translate[0], z, now);
    applyStyleProperty(PropertyKey::TextTranslateY, symbol.text.translate[1], z, now);
    applyStyleProperty(PropertyKey::TextTranslateAnchor, symbol.text.translate_anchor, z, now);

}

template <>
void StyleLayer::applyStyleProperties<RasterProperties>(const float z, const timestamp now) {
    properties.set<RasterProperties>();
    RasterProperties &raster = properties.get<RasterProperties>();
    applyStyleProperty(PropertyKey::RasterOpacity, raster.opacity, z, now);
    applyStyleProperty(PropertyKey::RasterHueRotate, raster.hue_rotate, z, now);
    applyStyleProperty(PropertyKey::RasterBrightnessLow, raster.brightness[0], z, now);
    applyStyleProperty(PropertyKey::RasterBrightnessHigh, raster.brightness[1], z, now);
    applyStyleProperty(PropertyKey::RasterSaturation, raster.saturation, z, now);
    applyStyleProperty(PropertyKey::RasterContrast, raster.contrast, z, now);
    applyStyleProperty(PropertyKey::RasterFade, raster.fade, z, now);
}

template <>
void StyleLayer::applyStyleProperties<BackgroundProperties>(const float z, const timestamp now) {
    properties.set<BackgroundProperties>();
    BackgroundProperties &background = properties.get<BackgroundProperties>();
    applyStyleProperty(PropertyKey::BackgroundColor, background.color, z, now);
}

void StyleLayer::updateProperties(float z, const timestamp now) {
    if (layers) {
        layers->updateProperties(z, now);
    }

    cleanupAppliedStyleProperties(now);

    switch (type) {
        case StyleLayerType::Fill: applyStyleProperties<FillProperties>(z, now); break;
        case StyleLayerType::Line: applyStyleProperties<LineProperties>(z, now); break;
        case StyleLayerType::Symbol: applyStyleProperties<SymbolProperties>(z, now); break;
        case StyleLayerType::Raster: applyStyleProperties<RasterProperties>(z, now); break;
        case StyleLayerType::Background: applyStyleProperties<BackgroundProperties>(z, now); break;
        default: properties.set<std::false_type>(); break;
    }
}

bool StyleLayer::hasTransitions() const {
    for (const std::pair<PropertyKey, AppliedClassProperties> &pair : appliedStyle) {
        if (pair.second.hasTransitions()) {
            return true;
        }
    }
    return false;
}


void StyleLayer::cleanupAppliedStyleProperties(timestamp now) {
    auto it = appliedStyle.begin();
    const auto end = appliedStyle.end();
    while (it != end) {
        AppliedClassProperties &properties = it->second;
        properties.cleanup(now);

        // If the current properties object is empty, remove it from the map entirely.
        if (properties.empty()) {
            appliedStyle.erase(it++);
        } else {
            ++it;
        }
    }
}

}
