#pragma once

#include <mbgl/gl/object.hpp>

#include <array>

namespace mbgl {
namespace gl {

class Framebuffer {
public:
    std::array<uint16_t, 2> size;
    gl::UniqueFramebuffer framebuffer;
};

} // namespace gl
} // namespace mbgl
