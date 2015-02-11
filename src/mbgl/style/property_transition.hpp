#ifndef MBGL_STYLE_PROPERTY_TRANSITION
#define MBGL_STYLE_PROPERTY_TRANSITION

#include <cstdint>
#include <chrono>

namespace mbgl {

struct PropertyTransition {
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero();
    std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero();
};

}

#endif
