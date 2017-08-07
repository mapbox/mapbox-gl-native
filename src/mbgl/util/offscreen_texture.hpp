#pragma once

#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
class Texture;
} // namespace gl

enum class OffscreenTextureAttachment {
    None,
    Depth,
};

class OffscreenTexture {
public:
    OffscreenTexture(gl::Context&,
                     Size size = { 256, 256 },
                     OffscreenTextureAttachment type = OffscreenTextureAttachment::None);
    ~OffscreenTexture();
    OffscreenTexture(OffscreenTexture&&);
    OffscreenTexture& operator=(OffscreenTexture&&);

    void bind();

    PremultipliedImage readStillImage();

    gl::Texture& getTexture();

    const Size& getSize() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
