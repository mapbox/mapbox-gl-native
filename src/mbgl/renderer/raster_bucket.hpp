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

    void render(Painter &painter, const StyleLayer &layer_desc, const TileID &id,
                const mat4 &matrix) override;
    bool hasData() const override;

    bool setImage(const std::string &data);

    const StyleLayoutRaster &layout;

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array);

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, GLuint texture);

    Raster raster;
};

}

#endif
