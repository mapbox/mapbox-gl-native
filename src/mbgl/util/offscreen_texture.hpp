#pragma once

#include <mbgl/map/view.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class OffscreenTexture : public View {
public:
    OffscreenTexture(gl::Context&, std::array<uint16_t, 2> size = {{ 256, 256 }});

    void bind() override;

    PremultipliedImage readStillImage();

    gl::Texture& getTexture();

private:
    gl::Context& context;
    std::array<uint16_t, 2> size;
    optional<gl::Framebuffer> framebuffer;
    optional<gl::Texture> texture;
};

} // namespace mbgl
