#ifndef MBGL_STYLE_CLASS_PROPERTIES
#define MBGL_STYLE_CLASS_PROPERTIES

#include <mbgl/style/property_key.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_transition.hpp>

#include <map>

namespace mbgl {

class ClassProperties {
public:
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
