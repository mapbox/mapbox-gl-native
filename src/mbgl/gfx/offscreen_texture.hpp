#pragma once

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {
namespace gfx {

class Texture;

class OffscreenTexture : public gfx::Renderable {
protected:
    OffscreenTexture(const Size size_, std::unique_ptr<RenderableResource> resource_)
        : gfx::Renderable(size_, std::move(resource_)) {
    }

public:
    virtual ~OffscreenTexture() = default;

    virtual bool isRenderable() = 0;

    virtual PremultipliedImage readStillImage() = 0;
    virtual gfx::Texture& getTexture() = 0;
};

} // namespace gfx
} // namespace mbgl
