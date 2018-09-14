#pragma once

#include <cstdint>

namespace mbgl {
namespace gl {

class CullFaceMode {
public:
    enum CullFace : bool {
        Disable = false,
        Enable = true,
    };

    enum CullFaceSide : uint32_t {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408,
    };

    enum FrontFace : uint32_t {
        Clockwise = 0x0900,
        CounterClockwise = 0x0901
    };

    CullFace cullFace;
    CullFaceSide side;
    FrontFace frontFace;

    static CullFaceMode disabled() {
        return CullFaceMode { Disable, CullFaceSide::Back, FrontFace::CounterClockwise };
    }

    static CullFaceMode backCCW() {
        return CullFaceMode { Enable, CullFaceSide::Back, FrontFace::CounterClockwise };
    }
};

} // namespace gl
} // namespace mbgl
