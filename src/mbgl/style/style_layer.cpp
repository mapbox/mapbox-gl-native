#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/property_fallback.hpp>

#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/layer/background_layer.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> StyleLayer::create(StyleLayerType type) {
    switch (type) {
    case StyleLayerType::Fill:
        return std::make_unique<FillLayer>();
    case StyleLayerType::Line:
        return std::make_unique<LineLayer>();
    case StyleLayerType::Circle:
        return std::make_unique<CircleLayer>();
    case StyleLayerType::Symbol:
        return std::make_unique<SymbolLayer>();
    case StyleLayerType::Raster:
        return std::make_unique<RasterLayer>();
    case StyleLayerType::Background:
        return std::make_unique<BackgroundLayer>();
    default:
        return nullptr;
    }
}

bool StyleLayer::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

void StyleLayer::setClasses(const std::vector<std::string> &class_names, const TimePoint& now,
                            const PropertyTransition &defaultTransition) {
    // Stores all keys that we have already added transitions for.
    std::set<PropertyKey> already_applied;

    // We only apply the default style values if there are no classes set.
    if (class_names.empty()) {
        applyClassProperties(ClassID::Default, already_applied, now, defaultTransition);
        return;
    }

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
        if (appliedProperties.mostRecent() == ClassID::Fallback) {
            continue;
        }

        // This property key hasn't been set by a previous class, so we need to add a transition
        // to the fallback value for that key.
        const TimePoint begin = now + defaultTransition.delay;
        const TimePoint end = begin + defaultTransition.duration;
        const PropertyValue &value = PropertyFallbackValue::Get(key);
        appliedProperties.add(ClassID::Fallback, begin, end, value);
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

void StyleLayer::updateProperties(const StyleCalculationParameters& parameters) {
    cleanupAppliedStyleProperties(parameters.now);

    // Clear the pending transitions flag upon each update.
    hasPendingTransitions = false;

    // Update the render passes when this layer is visible.
    passes = applyStyleProperties(parameters);
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
