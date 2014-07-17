#include <mbgl/style/class_properties.hpp>

namespace mbgl {

const PropertyTransition &ClassProperties::getTransition(PropertyKey key, const PropertyTransition &defaultTransition) const {
    auto it = transitions.find(key);
    if (it == transitions.end()) {
        return defaultTransition;
    } else {
        return it->second;
    }
}

}
