#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/property_fallback.hpp>

#include <mbgl/util/interpolate.hpp>

namespace mbgl {

StyleLayer::StyleLayer(const std::string &id_, std::map<ClassID, ClassProperties> &&styles_)
    : id(id_), styles(std::move(styles_)) {}

bool StyleLayer::isBackground() const {
    return type == StyleLayerType::Background;
}

bool StyleLayer::isVisible() const {
    switch (type) {
        case StyleLayerType::Fill:
            return getProperties<FillProperties>().isVisible();
        case StyleLayerType::Line:
            return getProperties<LineProperties>().isVisible();
        case StyleLayerType::Circle:
            return getProperties<CircleProperties>().isVisible();
        case StyleLayerType::Symbol:
            return getProperties<SymbolProperties>().isVisible();
        case StyleLayerType::Raster:
            return getProperties<RasterProperties>().isVisible();
        default:
            return false;
    }
}

bool StyleLayer::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

void StyleLayer::setClasses(const std::vector<std::string> &class_names, const TimePoint& now,
                            const PropertyTransition &defaultTransition) {
    // Stores all keys that we have already added transitions for.
    std::set<PropertyKey> already_applied;

    // Reverse iterate through all class names and apply them last to first.
    for (auto it = class_names.rbegin(); it != class_names.rend(); ++it) {
        const std::string &class_name = *it;
        // From here on, we're only dealing with IDs to avoid comparing strings all the time.
        const ClassID class_id = ClassDictionary::Get().lookup(class_name);
        applyClassProperties(class_id, already_applied, now, defaultTransition);
    }

    // As the last class, apply the default class.
    applyClassProperties(ClassID::Default, already_applied, now, defaultTransition);

    // Make sure that we also transition to the fallback value for keys that aren't changed by
    // any applied classes.
    for (auto& property_pair : appliedStyle) {
        const PropertyKey key = property_pair.first;
        if (already_applied.find(key) != already_applied.end()) {
            // This property has already been set by a previous class, so we don't need to
            // transition to the fallback.
            continue;
        }

        AppliedClassPropertyValues &appliedProperties = property_pair.second;
        // Make sure that we don't do double transitions to the fallback value.
        if (appliedProperties.mostRecent() != ClassID::Fallback) {
            // This property key hasn't been set by a previous class, so we need to add a transition
            // to the fallback value for that key.
            const TimePoint begin = now + defaultTransition.delay;
            const TimePoint end = begin + defaultTransition.duration;
            const PropertyValue &value = PropertyFallbackValue::Get(key);
            appliedProperties.add(ClassID::Fallback, begin, end, value);
        }
    }
}

// Helper function for applying all properties of a a single class that haven't been applied yet.
void StyleLayer::applyClassProperties(const ClassID class_id,
                                      std::set<PropertyKey> &already_applied, const TimePoint& now,
                                      const PropertyTransition &defaultTransition) {
    auto style_it = styles.find(class_id);
    if (style_it == styles.end()) {
        // There is no class in this layer with this class_name.
        return;
    }

    // Loop through all the properties in this style, and add transitions to them, if they're
    // not already the most recent transition.
    const ClassProperties &class_properties = style_it->second;
    for (const auto& property_pair : class_properties) {
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
        AppliedClassPropertyValues &appliedProperties = appliedStyle[key];
        if (appliedProperties.mostRecent() != class_id) {
            const PropertyTransition &transition =
                class_properties.getTransition(key, defaultTransition);
            const TimePoint begin = now + transition.delay;
            const TimePoint end = begin + transition.duration;
            const PropertyValue &value = property_pair.second;
            appliedProperties.add(class_id, begin, end, value);
        }
    }
}

template <typename T>
struct PropertyEvaluator {
    typedef T result_type;
    PropertyEvaluator(float z_, const ZoomHistory &zoomHistory_) : z(z_), zoomHistory(zoomHistory_) {}

    template <typename P, typename std::enable_if<std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &value) const {
        return value;
    }

    T operator()(const Function<T> &value) const {
        return mapbox::util::apply_visitor(FunctionEvaluator<T>(z), value);
    }

    T operator()(const PiecewiseConstantFunction<T> &value) const {
        return value.evaluate(z, zoomHistory);
    }

    template <typename P, typename std::enable_if<!std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &) const {
        return T();
    }

private:
    const float z;
    const ZoomHistory &zoomHistory;
};

template <typename T>
void StyleLayer::applyStyleProperty(PropertyKey key, T &target, const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    auto it = appliedStyle.find(key);
    if (it != appliedStyle.end()) {
        AppliedClassPropertyValues &applied = it->second;
        // Iterate through all properties that we need to apply in order.
        const PropertyEvaluator<T> evaluator(z, zoomHistory);
        for (auto& property : applied.propertyValues) {
            if (now >= property.begin) {
                // We overwrite the current property with the new value.
                target = mapbox::util::apply_visitor(evaluator, property.value);
            } else {
                // Do not apply this property because its transition hasn't begun yet.
            }
        }
    }
}

template <typename T>
void StyleLayer::applyTransitionedStyleProperty(PropertyKey key, T &target, const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    auto it = appliedStyle.find(key);
    if (it != appliedStyle.end()) {
        AppliedClassPropertyValues &applied = it->second;
        // Iterate through all properties that we need to apply in order.
        const PropertyEvaluator<T> evaluator(z, zoomHistory);
        for (auto& property : applied.propertyValues) {
            if (now >= property.end) {
                // We overwrite the current property with the new value.
                target = mapbox::util::apply_visitor(evaluator, property.value);
            } else if (now >= property.begin) {
                // We overwrite the current property partially with the new value.
                float progress = std::chrono::duration<float>(now - property.begin) / (property.end - property.begin);
                target = util::interpolate(target, mapbox::util::apply_visitor(evaluator, property.value), progress);
                hasPendingTransitions = true;
            } else {
                // Do not apply this property because its transition hasn't begun yet.
            }
        }
    }
}

template <>
void StyleLayer::applyStyleProperties<FillProperties>(const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    properties.set<FillProperties>();
    FillProperties &fill = properties.get<FillProperties>();
    applyStyleProperty(PropertyKey::FillAntialias, fill.antialias, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillOpacity, fill.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillColor, fill.fill_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillOutlineColor, fill.stroke_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillTranslate, fill.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::FillTranslateAnchor, fill.translateAnchor, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::FillImage, fill.image, z, now, zoomHistory);
}

template <>
void StyleLayer::applyStyleProperties<LineProperties>(const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    properties.set<LineProperties>();
    LineProperties &line = properties.get<LineProperties>();
    applyTransitionedStyleProperty(PropertyKey::LineOpacity, line.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineColor, line.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineTranslate, line.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::LineTranslateAnchor, line.translateAnchor, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineWidth, line.width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineGapWidth, line.gap_width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineBlur, line.blur, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::LineDashArray, line.dash_array, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::LineImage, line.image, z, now, zoomHistory);

    // for scaling dasharrays
    applyStyleProperty(PropertyKey::LineWidth, line.dash_line_width, std::floor(z), now, zoomHistory);
}

template <>
void StyleLayer::applyStyleProperties<CircleProperties>(const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    properties.set<CircleProperties>();
    CircleProperties& circle = properties.get<CircleProperties>();
    applyTransitionedStyleProperty(PropertyKey::CircleRadius, circle.radius, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleColor, circle.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleOpacity, circle.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleTranslate, circle.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::CircleTranslateAnchor, circle.translateAnchor, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleBlur, circle.blur, z, now, zoomHistory);
}

template <>
void StyleLayer::applyStyleProperties<SymbolProperties>(const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    properties.set<SymbolProperties>();
    SymbolProperties &symbol = properties.get<SymbolProperties>();
    applyTransitionedStyleProperty(PropertyKey::IconOpacity, symbol.icon.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconColor, symbol.icon.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconHaloColor, symbol.icon.halo_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconHaloWidth, symbol.icon.halo_width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconHaloBlur, symbol.icon.halo_blur, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconTranslate, symbol.icon.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::IconTranslateAnchor, symbol.icon.translate_anchor, z, now, zoomHistory);

    applyTransitionedStyleProperty(PropertyKey::TextOpacity, symbol.text.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextColor, symbol.text.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextHaloColor, symbol.text.halo_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextHaloWidth, symbol.text.halo_width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextHaloBlur, symbol.text.halo_blur, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextTranslate, symbol.text.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::TextTranslateAnchor, symbol.text.translate_anchor, z, now, zoomHistory);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    auto it = bucket->layout.properties.find(PropertyKey::IconSize);
    if (it != bucket->layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(z, zoomHistory);
        symbol.icon.size = mapbox::util::apply_visitor(evaluator, it->second);
    }
    it = bucket->layout.properties.find(PropertyKey::TextSize);
    if (it != bucket->layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(z, zoomHistory);
        symbol.text.size = mapbox::util::apply_visitor(evaluator, it->second);
    }
}

template <>
void StyleLayer::applyStyleProperties<RasterProperties>(const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    properties.set<RasterProperties>();
    RasterProperties &raster = properties.get<RasterProperties>();
    applyTransitionedStyleProperty(PropertyKey::RasterOpacity, raster.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterHueRotate, raster.hue_rotate, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterBrightnessLow, raster.brightness[0], z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterBrightnessHigh, raster.brightness[1], z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterSaturation, raster.saturation, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterContrast, raster.contrast, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterFade, raster.fade, z, now, zoomHistory);
}

template <>
void StyleLayer::applyStyleProperties<BackgroundProperties>(const float z, const TimePoint& now, const ZoomHistory &zoomHistory) {
    properties.set<BackgroundProperties>();
    BackgroundProperties &background = properties.get<BackgroundProperties>();
    applyTransitionedStyleProperty(PropertyKey::BackgroundOpacity, background.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::BackgroundColor, background.color, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::BackgroundImage, background.image, z, now, zoomHistory);
}

void StyleLayer::updateProperties(float z, const TimePoint& now, ZoomHistory &zoomHistory) {
    cleanupAppliedStyleProperties(now);

    // Clear the pending transitions flag upon each update.
    hasPendingTransitions = false;

    switch (type) {
        case StyleLayerType::Fill: applyStyleProperties<FillProperties>(z, now, zoomHistory); break;
        case StyleLayerType::Line: applyStyleProperties<LineProperties>(z, now, zoomHistory); break;
        case StyleLayerType::Circle: applyStyleProperties<CircleProperties>(z, now, zoomHistory); break;
        case StyleLayerType::Symbol: applyStyleProperties<SymbolProperties>(z, now, zoomHistory); break;
        case StyleLayerType::Raster: applyStyleProperties<RasterProperties>(z, now, zoomHistory); break;
        case StyleLayerType::Background: applyStyleProperties<BackgroundProperties>(z, now, zoomHistory); break;
        default: properties.set<std::false_type>(); break;
    }

    // Update the render passes when this layer is visible.
    passes = RenderPass::None;
    if (isVisible()) {
        if (properties.is<FillProperties>()) {
            const FillProperties &fillProperties = properties.get<FillProperties>();
            const float alpha = fillProperties.fill_color[3] * fillProperties.opacity;

            if (fillProperties.antialias) {
                passes |= RenderPass::Translucent;
            }
            if (!fillProperties.image.from.empty() || alpha < 1.0f) {
                passes |= RenderPass::Translucent;
            } else {
                passes |= RenderPass::Opaque;
            }
        } else {
            passes |= RenderPass::Translucent;
        }
    }
}

bool StyleLayer::hasTransitions() const {
    for (const auto& pair : appliedStyle) {
        if (pair.second.hasTransitions()) {
            return true;
        }
    }
    return hasPendingTransitions;
}

void StyleLayer::cleanupAppliedStyleProperties(const TimePoint& now) {
    for (auto it = appliedStyle.begin(); it != appliedStyle.end();) {
        AppliedClassPropertyValues& values = it->second;
        values.cleanup(now);
        // If the current properties object is empty, remove it from the map entirely.
        values.empty() ? appliedStyle.erase(it++) : ++it;
    }
}

}
