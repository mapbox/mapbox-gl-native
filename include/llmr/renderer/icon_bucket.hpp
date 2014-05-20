#ifndef LLMR_RENDERER_ICONBUCKET
#define LLMR_RENDERER_ICONBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/icon_buffer.hpp>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace llmr {

class Style;
class IconVertexBuffer;
class BucketDescription;
class IconShader;
class DotShader;
class SpriteAtlas;
class VectorTileFeature;

class IconBucket : public Bucket {
public:
    IconBucket(IconVertexBuffer& vertexBuffer,
                const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);
    virtual bool hasData() const;

    void addFeature(const VectorTileFeature &feature, SpriteAtlas &sprite_atlas);

    void drawIcons(IconShader& shader);
    void drawIcons(DotShader& shader);

public:
    const BucketGeometryDescription geometry;

private:

    IconVertexBuffer& vertexBuffer;
    VertexArrayObject array;

    const size_t vertex_start;
    size_t vertex_end = 0;
};

}

#endif
