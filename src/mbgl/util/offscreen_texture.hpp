#pragma once

#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
class Texture;
} // namespace gl

class OffscreenTexture {
public:
    OffscreenTexture(gl::Context&,
                     Size size = { 256, 256 });
    ~OffscreenTexture();
    OffscreenTexture(OffscreenTexture&&);
    OffscreenTexture& operator=(OffscreenTexture&&);

    void bind();
    void attachRenderbuffer(gl::Renderbuffer<gl::RenderbufferType::DepthComponent>&);

    PremultipliedImage readStillImage();

    gl::Texture& getTexture();

    const Size& getSize() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
