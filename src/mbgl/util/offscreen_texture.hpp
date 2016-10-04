#pragma once

#include <mbgl/gl/texture.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class OffscreenTexture {
public:
    void bind(gl::Context&, std::array<uint16_t, 2> size);

    gl::Texture& getTexture();
    std::array<uint16_t, 2> getSize() const;

private:
    optional<gl::UniqueFramebuffer> framebuffer;
    optional<gl::Texture> texture;
};

} // namespace mbgl
