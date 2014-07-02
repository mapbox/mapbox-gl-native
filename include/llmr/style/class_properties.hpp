#ifndef LLMR_STYLE_CLASS_PROPERTIES
#define LLMR_STYLE_CLASS_PROPERTIES

#include <llmr/style/property_key.hpp>
#include <llmr/style/property_value.hpp>
#include <llmr/style/property_transition.hpp>

#include <map>

namespace llmr {

class ClassProperties {
public:
    inline ClassProperties() {}
    inline ClassProperties(ClassProperties &&properties)
        : properties(std::move(properties.properties)) {}

    inline void set(PropertyKey key, const PropertyValue &value) {
        properties.emplace(key, value);
    }

    inline void set(PropertyKey key, const PropertyTransition &transition) {
        transitions.emplace(key, transition);
    }

    const PropertyTransition &getTransition(PropertyKey key, const PropertyTransition &defaultTransition) const;

    // Route-through iterable interface so that you can iterate on the object as is.
    inline std::map<PropertyKey, PropertyValue>::const_iterator begin() const {
        return properties.begin();
    }
    inline std::map<PropertyKey, PropertyValue>::const_iterator end() const {
        return properties.end();
    }

public:
    std::map<PropertyKey, PropertyValue> properties;
    std::map<PropertyKey, PropertyTransition> transitions;
};

}

#endif
