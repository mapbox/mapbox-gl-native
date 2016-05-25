#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/raster.hpp>

namespace mbgl {

class RasterShader;
class StaticVertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(gl::TexturePool&);

    void upload(gl::GLObjectStore&) override;
    void render(Painter&, const StyleLayer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void setImage(PremultipliedImage);

    void drawRaster(RasterShader&, StaticVertexBuffer&, VertexArrayObject&, gl::GLObjectStore&);

    Raster raster;
};

} // namespace mbgl
