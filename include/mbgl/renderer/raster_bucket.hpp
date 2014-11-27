#ifndef MBGL_RENDERER_RASTERBUCKET
#define MBGL_RENDERER_RASTERBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/renderer/prerendered_texture.hpp>
#include <mbgl/style/style_bucket.hpp>



namespace mbgl {

class RasterShader;
class StaticVertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(Texturepool&, const StyleBucketRaster&);

    virtual void render(Painter& painter, util::ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    virtual bool hasData() const;

    bool setImage(const std::string &data);

    const StyleBucketRaster &properties;
    PrerenderedTexture texture;

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array);

    void drawRaster(RasterShader& shader, StaticVertexBuffer &vertices, VertexArrayObject &array, GLuint texture);

    Raster raster;
};

}

#endif
