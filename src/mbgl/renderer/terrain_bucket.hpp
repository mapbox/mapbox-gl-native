#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/geometry/dem_pyramid.hpp>

namespace mbgl {

class TerrainPrepareShader;
class TerrainShader;
class RasterShader;
class RasterVertex;

namespace gl {
class Context;
template <class> class VertexBuffer;
class VertexArrayObject;
} // namespace gl

class TerrainBucket : public Bucket {
public:
    TerrainBucket(PremultipliedImage&&);
    TerrainBucket(DEMPyramid&&);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;

    optional<gl::Texture> dem;
    optional<gl::Texture> texture;
    bool prepared = false;

private:
    DEMPyramid pyramid;
};

} // namespace mbgl
