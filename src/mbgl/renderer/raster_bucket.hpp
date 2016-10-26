#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gl/texture.hpp>

namespace mbgl {

class RasterShader;
class RasterVertex;

namespace gl {
class Context;
template <class> class VertexBuffer;
class VertexArrayObject;
} // namespace gl

class RasterBucket : public Bucket {
public:
    RasterBucket(PremultipliedImage&&);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void drawRaster(RasterShader&, gl::VertexBuffer<RasterVertex>&, gl::VertexArrayObject&, gl::Context&);

private:
    PremultipliedImage image;
    optional<gl::Texture> texture;
};

} // namespace mbgl
