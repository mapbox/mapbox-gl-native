#ifndef LLMR_STYLE_CLASS_PROPERTIES
#define LLMR_STYLE_CLASS_PROPERTIES

#include <llmr/style/property_key.hpp>
#include <llmr/style/property_value.hpp>

#include <map>

namespace llmr {

struct ClassPropertyTransition {
    uint16_t duration = 0;
    uint16_t delay = 0;
};

class ClassProperties {
public:
    inline ClassProperties() {}
    inline ClassProperties(ClassProperties &&properties)
        : properties(std::move(properties.properties)) {}

    template <typename ...Args>
    inline void set(Args&& ...args) {
        properties.emplace(::std::forward<Args>(args)...);
    }
    template <typename ...Args>
    inline void setTransition(Args&& ...args) {
        transitions.emplace(::std::forward<Args>(args)...);
    }

    inline const ClassPropertyTransition &getTransition(PropertyKey key, const ClassPropertyTransition &defaultTransition) const {
        auto it = transitions.find(key);
        if (it == transitions.end()) {
            return defaultTransition;
        } else {
            return it->second;
        }
    }

    // Route-through iterable interface so that you can iterate on the object as is.
    inline std::map<PropertyKey, PropertyValue>::const_iterator begin() const {
        return properties.begin();
    }
    inline std::map<PropertyKey, PropertyValue>::const_iterator end() const {
        return properties.end();
    }

public:
    std::map<PropertyKey, PropertyValue> properties;
    std::map<PropertyKey, ClassPropertyTransition> transitions;
};

}

#endif
