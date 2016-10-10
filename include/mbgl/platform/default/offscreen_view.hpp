#pragma once

#include <mbgl/map/view.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class OffscreenView : public View {
public:
    OffscreenView(gl::Context&, std::array<uint16_t, 2> size = {{ 256, 256 }});

    void bind() override;

    PremultipliedImage readStillImage();

    std::array<uint16_t, 2> getSize() const;

private:
    gl::Context& context;
    std::array<uint16_t, 2> size;
    optional<gl::Framebuffer> framebuffer;
    optional<gl::Renderbuffer<gl::RenderbufferType::RGBA>> color;
    optional<gl::Renderbuffer<gl::RenderbufferType::DepthStencil>> depthStencil;
};

} // namespace mbgl
