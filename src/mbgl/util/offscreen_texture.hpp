#pragma once

#include <mbgl/map/view.hpp>
//<<<<<<< HEAD
//=======
//#include <mbgl/gl/framebuffer.hpp>
//#include <mbgl/gl/renderbuffer.hpp>
//#include <mbgl/gl/texture.hpp>
//#include <mbgl/util/optional.hpp>
//>>>>>>> wip: rendering to external texture/renderbuffers, but why are there stripes :sob:
#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
class Texture;
} // namespace gl

class OffscreenTexture : public View {
public:
    OffscreenTexture(gl::Context&, Size size = { 256, 256 });
    ~OffscreenTexture();

    void bind() override;
    void bindRenderbuffers();

    PremultipliedImage readStillImage();

    gl::Texture& getTexture();

    const Size& getSize() const;

private:
//<<<<<<< HEAD
    class Impl;
    const std::unique_ptr<Impl> impl;
//=======
//    gl::Context& context;
//    optional<gl::Framebuffer> framebuffer;
//    optional<gl::Texture> texture;
//    optional<gl::Renderbuffer<gl::RenderbufferType::RGBA>> colorTarget;
//    optional<gl::Renderbuffer<gl::RenderbufferType::DepthComponent>> depthTarget;
//>>>>>>> wip: rendering to external texture/renderbuffers, but why are there stripes :sob:
};

} // namespace mbgl
