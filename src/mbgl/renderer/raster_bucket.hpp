#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gl/texture.hpp>

namespace mbgl {

namespace style {
class Style;
} // namespace style

class RasterBucket : public Bucket {
public:
    RasterBucket(UnassociatedImage&&);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const RenderLayer&, const RenderTile&, const style::Style&) override;
    bool hasData() const override;

    UnassociatedImage image;
    optional<gl::Texture> texture;
};

} // namespace mbgl
