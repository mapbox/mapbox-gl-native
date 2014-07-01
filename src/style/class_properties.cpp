#include <llmr/style/class_properties.hpp>

namespace llmr {

const PropertyTransition &ClassProperties::getTransition(PropertyKey key, const PropertyTransition &defaultTransition) const {
    auto it = transitions.find(key);
    if (it == transitions.end()) {
        fprintf(stderr, "using default transition: %d/%d\n", defaultTransition.duration, defaultTransition.delay);
        return defaultTransition;
    } else {
        return it->second;
    }
}

}
