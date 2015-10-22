#include <mbgl/style/class_properties.hpp>

namespace mbgl {

PropertyTransition ClassProperties::getTransition(PropertyKey key) const {
    auto it = transitions.find(key);
    if (it == transitions.end()) {
        return PropertyTransition();
    } else {
        return it->second;
    }
}

}
