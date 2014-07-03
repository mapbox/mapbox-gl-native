#ifndef LLMR_RENDERER_ICONBUCKET
#define LLMR_RENDERER_ICONBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/icon_buffer.hpp>
#include <llmr/style/style_bucket.hpp>

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
                const StyleBucketIcon& properties);

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id);
    virtual bool hasData() const;

    void addFeature(const VectorTileFeature &feature, SpriteAtlas &sprite_atlas);

    void drawIcons(IconShader& shader);
    void drawIcons(DotShader& shader);

public:
    const StyleBucketIcon &properties;

private:

    IconVertexBuffer& vertexBuffer;
    VertexArrayObject array;

    const size_t vertex_start;
    size_t vertex_end = 0;
};

}

#endif
