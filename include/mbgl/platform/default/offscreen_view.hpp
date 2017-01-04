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
    OffscreenView(gl::Context&, Size size = { 256, 256 });

    void bind() override;

    PremultipliedImage readStillImage();

public:
    const Size size;

private:
    gl::Context& context;
    optional<gl::Framebuffer> framebuffer;
    optional<gl::Renderbuffer<gl::RenderbufferType::RGBA>> color;
    optional<gl::Renderbuffer<gl::RenderbufferType::DepthStencil>> depthStencil;
};

} // namespace mbgl
