#ifndef MBGL_STYLE_PROPERTY_TRANSITION
#define MBGL_STYLE_PROPERTY_TRANSITION

#include <mbgl/util/chrono.hpp>

#include <mapbox/optional.hpp>
#include <cstdint>

namespace mbgl {

class PropertyTransition {
public:
    mapbox::util::optional<Duration> duration;
    mapbox::util::optional<Duration> delay;
};

}

#endif
