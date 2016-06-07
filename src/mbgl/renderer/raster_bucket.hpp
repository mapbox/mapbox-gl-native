#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {

class RasterRenderable;

namespace gl { class TexturePool; }

class RasterBucket : public Bucket {
public:
    RasterBucket(gl::TexturePool&);
    ~RasterBucket() override;

    RasterRenderable& getRenderable() const;

    void upload(gl::ObjectStore&) override;
    void render(Painter&, const style::Layer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void setImage(PremultipliedImage);

private:
    std::unique_ptr<RasterRenderable> renderable;
};

} // namespace mbgl
