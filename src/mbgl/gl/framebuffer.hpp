#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gl {

class Framebuffer {
public:
    Size size;
    gl::UniqueFramebuffer framebuffer;
};

} // namespace gl
} // namespace mbgl
