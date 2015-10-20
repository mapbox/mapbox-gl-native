#include <mbgl/style/paint_properties_map.hpp>
#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/property_fallback.hpp>
#include <mbgl/style/class_properties.hpp>

namespace mbgl {

void PaintPropertiesMap::parseEach(const JSVal& layer, std::function<void (ClassProperties &, const JSVal &)> parsePaint) {
    rapidjson::Value::ConstMemberIterator itr = layer.MemberBegin();
    for (; itr != layer.MemberEnd(); ++itr) {
        const std::string name { itr->name.GetString(), itr->name.GetStringLength() };
        if (name == "paint") {
            parsePaint(paints[ClassID::Default], itr->value);
        } else if (name.compare(0, 6, "paint.") == 0 && name.length() > 6) {
            parsePaint(paints[ClassDictionary::Get().lookup(name.substr(6))], itr->value);
        }
    }
}

void PaintPropertiesMap::cascade(const std::vector<std::string>& classes,
                                 const TimePoint& now,
                                 const PropertyTransition& defaultTransition) {
    // Stores all keys that we have already added transitions for.
    std::set<PropertyKey> alreadyApplied;

    // We only apply the default style values if there are no classes set.
    if (classes.empty()) {
        cascadeClass(ClassID::Default, alreadyApplied, now, defaultTransition);
        return;
    }

    // Reverse iterate through all class names and apply them last to first.
    for (auto it = classes.rbegin(); it != classes.rend(); ++it) {
        // From here on, we're only dealing with IDs to avoid comparing strings all the time.
        cascadeClass(ClassDictionary::Get().lookup(*it), alreadyApplied, now, defaultTransition);
    }

    // As the last class, apply the default class.
    cascadeClass(ClassID::Default, alreadyApplied, now, defaultTransition);

    // Make sure that we also transition to the fallback value for keys that aren't changed by
    // any applied classes.
    for (auto& propertyPair : appliedStyle) {
        const PropertyKey key = propertyPair.first;
        if (alreadyApplied.find(key) != alreadyApplied.end()) {
            // This property has already been set by a previous class, so we don't need to
            // transition to the fallback.
            continue;
        }

        AppliedClassPropertyValues &appliedProperties = propertyPair.second;
        // Make sure that we don't do double transitions to the fallback value.
        if (appliedProperties.mostRecent() == ClassID::Fallback) {
            continue;
        }

        // This property key hasn't been set by a previous class, so we need to add a transition
        // to the fallback value for that key.
        const TimePoint begin = now + *defaultTransition.delay;
        const TimePoint end = begin + *defaultTransition.duration;
        const PropertyValue &value = PropertyFallbackValue::Get(key);
        appliedProperties.add(ClassID::Fallback, begin, end, value);
    }
}

void PaintPropertiesMap::cascadeClass(const ClassID classID,
                                      std::set<PropertyKey>& alreadyApplied,
                                      const TimePoint& now,
                                      const PropertyTransition& defaultTransition) {
    auto styleIt = paints.find(classID);
    if (styleIt == paints.end()) {
        // There is no class in this layer with this class_name.
        return;
    }

    // Loop through all the properties in this style, and add transitions to them, if they're
    // not already the most recent transition.
    const ClassProperties& classProperties = styleIt->second;
    for (const auto& propertyPair : classProperties) {
        PropertyKey key = propertyPair.first;
        if (alreadyApplied.find(key) != alreadyApplied.end()) {
            // This property has already been set by a previous class.
            continue;
        }

        // Mark this property as written by a previous class, so that subsequent
        // classes won't override this.
        alreadyApplied.insert(key);

        // If the most recent transition is not the one with the highest priority, create
        // a transition.
        AppliedClassPropertyValues &appliedProperties = appliedStyle[key];
        if (appliedProperties.mostRecent() != classID) {
            PropertyTransition transition = classProperties.getTransition(key);
            Duration delay = transition.delay ? *transition.delay : *defaultTransition.delay;
            Duration duration = transition.duration ? *transition.duration : *defaultTransition.duration;
            const TimePoint begin = now + delay;
            const TimePoint end = begin + duration;
            const PropertyValue &value = propertyPair.second;
            appliedProperties.add(classID, begin, end, value);
        }
    }
}

bool PaintPropertiesMap::hasTransitions() const {
    for (const auto& pair : appliedStyle) {
        if (pair.second.hasTransitions()) {
            return true;
        }
    }
    return hasPendingTransitions;
}

void PaintPropertiesMap::removeExpiredTransitions(const TimePoint& now) {
    for (auto it = appliedStyle.begin(); it != appliedStyle.end();) {
        AppliedClassPropertyValues& values = it->second;
        values.cleanup(now);
        // If the current properties object is empty, remove it from the map entirely.
        values.empty() ? appliedStyle.erase(it++) : ++it;
    }

    // Clear the pending transitions flag upon each update.
    hasPendingTransitions = false;
}

}
