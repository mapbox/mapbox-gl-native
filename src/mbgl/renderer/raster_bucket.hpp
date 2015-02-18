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

    virtual void render(Painter& painter, util::ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    virtual bool hasData() const;

    bool setImage(const std::string &data);

    const StyleLayoutRaster &layout;

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array);

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, GLuint texture);

    Raster raster;
};

}

#endif
