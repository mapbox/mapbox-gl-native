#ifndef LLMR_RENDERER_RASTERBUCKET
#define LLMR_RENDERER_RASTERBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/util/raster.hpp>

namespace llmr {

class RasterShader;
class VertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(const std::shared_ptr<Texturepool> &texturepool);

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id);
    virtual bool hasData() const;

    bool setImage(const std::string &data);

    void drawRaster(RasterShader& shader, VertexBuffer &vertices, VertexArrayObject &array);

private:
    Raster raster;
};

}

#endif
