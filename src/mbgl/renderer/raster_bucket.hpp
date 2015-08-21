#ifndef MBGL_RENDERER_RASTERBUCKET
#define MBGL_RENDERER_RASTERBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/style/style_bucket.hpp>

namespace mbgl {

class StyleLayoutRaster;
class RasterShader;
class StaticVertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(TexturePool&, const StyleLayoutRaster&);

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;
    bool hasData() const;

    bool setImage(std::unique_ptr<util::Image> image);

    const StyleLayoutRaster &layout;

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array);

    Raster raster;
};

}

#endif
