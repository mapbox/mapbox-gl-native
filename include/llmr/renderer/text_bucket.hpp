#ifndef LLMR_RENDERER_TEXTBUCKET
#define LLMR_RENDERER_TEXTBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/text/types.hpp>
#include <memory>
#include <map>
#include <vector>

namespace llmr {

class Style;
class TextVertexBuffer;
class TriangleElementsBuffer;
class TextShader;
class Placement;
struct pbf;

class TextBucket : public Bucket {
    typedef ElementGroup triangle_group_type;

public:
    TextBucket(
        const std::shared_ptr<TextVertexBuffer> &vertexBuffer,
        const std::shared_ptr<TriangleElementsBuffer> &triangleElementsBuffer,
        const BucketDescription &bucket_desc, Placement &placement);

    virtual void render(Painter &painter, const std::string &layer_name,
                        const Tile::ID &id);

    void addGlyphs(const PlacedGlyphs &glyphs, float placementZoom,
                   PlacementRange placementRange, float zoom);

    void addFeature(const VectorTileFeature &feature,
                    const IndexedFaces &faces,
                    const std::map<Value, Shaping> &shapings);

    bool empty() const;

    void drawGlyphs(TextShader &shader);

public:
    const BucketGeometryDescription &geom_desc;

private:
    std::shared_ptr<TextVertexBuffer> vertexBuffer;
    std::shared_ptr<TriangleElementsBuffer> triangleElementsBuffer;
    Placement &placement;

    const size_t vertex_start;
    const size_t triangle_elements_start;

    std::vector<triangle_group_type> triangleGroups;
};
}

#endif
