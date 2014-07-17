#ifndef MBGL_STYLE_PROPERTY_TRANSITION
#define MBGL_STYLE_PROPERTY_TRANSITION

#include <cstdint>

namespace mbgl {

struct PropertyTransition {
    uint16_t duration = 0;
    uint16_t delay = 0;
};

}

#endif