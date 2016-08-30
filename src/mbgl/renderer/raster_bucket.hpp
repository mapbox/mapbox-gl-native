#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/gl/gl_config.hpp>

namespace mbgl {

class RasterShader;
class StaticRasterVertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    void upload(gl::ObjectStore&, gl::Config&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void setImage(PremultipliedImage);

    void drawRaster(RasterShader&, StaticRasterVertexBuffer&, VertexArrayObject&, gl::Config&, gl::ObjectStore&);

    Raster raster;
};

} // namespace mbgl
