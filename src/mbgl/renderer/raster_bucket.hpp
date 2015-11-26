#ifndef MBGL_RENDERER_RASTERBUCKET
#define MBGL_RENDERER_RASTERBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/raster.hpp>

namespace mbgl {

class RasterShader;
class StaticVertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(TexturePool&);

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;
    bool hasData() const override;

    bool setImage(PremultipliedImage);

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array);

    Raster raster;
};

}

#endif
