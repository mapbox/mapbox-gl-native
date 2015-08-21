#ifndef MBGL_STYLE_PROPERTY_FALLBACK
#define MBGL_STYLE_PROPERTY_FALLBACK

#include <mbgl/style/property_key.hpp>
#include <mbgl/style/property_value.hpp>

#include <map>

namespace mbgl {

class PropertyFallbackValue {
public:
    static const PropertyValue &Get(PropertyKey key) {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return it->second;
        } else {
            return defaultProperty;
        }
    }

private:
    static const std::map<PropertyKey, PropertyValue> properties;
    static const PropertyValue defaultProperty;
};

}

#endif
