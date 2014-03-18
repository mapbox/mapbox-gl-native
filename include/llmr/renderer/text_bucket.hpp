#ifndef LLMR_RENDERER_TEXTBUCKET
#define LLMR_RENDERER_TEXTBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <memory>
#include <map>
#include <vector>

namespace llmr {

class Style;
class TextVertexBuffer;
class TriangleElementsBuffer;
class TextShader;
class VectorTileFeature;
class VectorTileFace;
class VectorTilePlacement;
struct pbf;

class TextBucket : public Bucket {
    typedef ElementGroup triangle_group_type;
public:
    TextBucket(const std::shared_ptr<TextVertexBuffer>& vertexBuffer,
               const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
               const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addFeature(const VectorTileFeature& feature,
                    const std::vector<const VectorTileFace *>& faces,
                    const std::map<Value, std::vector<VectorTilePlacement>>& shaping);

    void addLabel(const std::vector<Coordinate>& line,
                  const std::vector<const VectorTileFace *>& faces,
                  const std::vector<VectorTilePlacement>& placements);

    bool empty() const;

    void drawGlyphs(TextShader& shader);

public:
    const BucketGeometryDescription geometry;

private:

    std::shared_ptr<TextVertexBuffer> vertexBuffer;
    std::shared_ptr<TriangleElementsBuffer> triangleElementsBuffer;

    const size_t vertex_start;
    const size_t triangle_elements_start;

    std::vector<triangle_group_type> triangleGroups;
};

}

#endif
