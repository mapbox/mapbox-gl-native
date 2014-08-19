#ifndef MBGL_STYLE_CLASS_PROPERTIES
#define MBGL_STYLE_CLASS_PROPERTIES

#include <mbgl/style/property_key.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_transition.hpp>

#include <map>

namespace mbgl {

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

}

#endif
