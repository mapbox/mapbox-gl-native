#ifndef MBGL_STYLE_PROPERTY_TRANSITION
#define MBGL_STYLE_PROPERTY_TRANSITION

#include <mbgl/util/chrono.hpp>

#include <cstdint>

namespace mbgl {

struct PropertyTransition {
    Duration duration = Duration::zero();
    Duration delay = Duration::zero();
};

}

#endif
