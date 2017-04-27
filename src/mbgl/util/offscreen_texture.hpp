#pragma once

#include <mbgl/map/view.hpp>
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
    void bindRenderbuffers(gl::TextureUnit unit = 0);

    PremultipliedImage readStillImage();

    gl::Texture& getTexture();

    const Size& getSize() const;

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
