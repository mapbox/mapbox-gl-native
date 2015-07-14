#ifndef MBGL_STYLE_PROPERTY_TRANSITION
#define MBGL_STYLE_PROPERTY_TRANSITION

#include <mbgl/util/chrono.hpp>

#include <cstdint>

namespace mbgl {

struct PropertyTransition {
    explicit inline PropertyTransition(const Duration& duration_, const Duration& delay_)
        : duration(duration_), delay(delay_) {}
    Duration duration = Duration::zero();
    Duration delay = Duration::zero();
};

}

#endif
