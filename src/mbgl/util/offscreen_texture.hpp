#pragma once

#include <mbgl/map/view.hpp>
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

class OffscreenTexture : public View {
public:
    OffscreenTexture(gl::Context&,
                     Size size = { 256, 256 },
                     OffscreenTextureAttachment type = OffscreenTextureAttachment::None);
    ~OffscreenTexture();
    OffscreenTexture(OffscreenTexture&&);
    OffscreenTexture& operator=(OffscreenTexture&&);

    void bind() override;

    PremultipliedImage readStillImage();

    gl::Texture& getTexture();

    const Size& getSize() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
