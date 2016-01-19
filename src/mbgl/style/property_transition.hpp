#ifndef MBGL_STYLE_PROPERTY_TRANSITION
#define MBGL_STYLE_PROPERTY_TRANSITION

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>

namespace mbgl {

class PropertyTransition {
public:
    optional<Duration> duration;
    optional<Duration> delay;
};

} // namespace mbgl

#endif
