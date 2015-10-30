#ifndef MBGL_STYLE_CLASS_PROPERTIES
#define MBGL_STYLE_CLASS_PROPERTIES

#include <mbgl/style/property_key.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluator.hpp>
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

    PropertyTransition getTransition(PropertyKey key) const;

    // Route-through iterable interface so that you can iterate on the object as is.
    inline std::map<PropertyKey, PropertyValue>::const_iterator begin() const {
        return properties.begin();
    }
    inline std::map<PropertyKey, PropertyValue>::const_iterator end() const {
        return properties.end();
    }

    template <typename T>
    void calculate(PropertyKey key, T& target, const float z) const {
        auto it = properties.find(key);
        if (it != properties.end()) {
            const PropertyEvaluator<T> evaluator(z);
            target = mapbox::util::apply_visitor(evaluator, it->second);
        }
    }

public:
    std::map<PropertyKey, PropertyValue> properties;
    std::map<PropertyKey, PropertyTransition> transitions;
};

}

#endif
