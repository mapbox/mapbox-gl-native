#ifndef LLMR_STYLE_PROPERTY_TRANSITION
#define LLMR_STYLE_PROPERTY_TRANSITION

#include <cstdint>

namespace llmr {

struct PropertyTransition {
    uint16_t duration = 0;
    uint16_t delay = 0;
};

}

#endif