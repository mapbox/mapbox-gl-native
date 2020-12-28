#pragma once

#include <mbgl/gfx/types.hpp>

namespace mbgl {
namespace gfx {

class CullFaceMode {
public:
    bool enabled;
    CullFaceSideType side;
    CullFaceWindingType winding;

    static CullFaceMode disabled() {
        return { false, CullFaceSideType::Back, CullFaceWindingType::CounterClockwise };
    }

    static CullFaceMode backCCW() {
        return { true, CullFaceSideType::Back, CullFaceWindingType::CounterClockwise };
    }
};

} // namespace gfx
} // namespace mbgl
