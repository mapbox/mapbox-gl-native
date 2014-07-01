#include <llmr/style/class_properties.hpp>

namespace llmr {

const PropertyTransition &ClassProperties::getTransition(PropertyKey key, const PropertyTransition &defaultTransition) const {
    auto it = transitions.find(key);
    if (it == transitions.end()) {
        return defaultTransition;
    } else {
        return it->second;
    }
}

}
