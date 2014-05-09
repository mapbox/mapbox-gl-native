#ifndef LLMR_MAP_VECTOR_TILE_DATA
#define LLMR_MAP_VECTOR_TILE_DATA

#include <llmr/map/tile_data.hpp>


#include <llmr/renderer/bucket.hpp>

#include <llmr/geometry/vertex_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>

#include <map>

namespace llmr {


class VectorTileData : public TileData {
    friend class TileParser;

public:
    VectorTileData(Tile::ID id, Map &map, const std::string url);
    ~VectorTileData();

    virtual void parse();
    virtual void render(Painter &painter, const LayerDescription& layer_desc);

protected:
    // Holds the actual geometries in this tile.
    FillVertexBuffer fillVertexBuffer;
    LineVertexBuffer lineVertexBuffer;
    PointVertexBuffer pointVertexBuffer;
    TextVertexBuffer textVertexBuffer;

    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;
    PointElementsBuffer pointElementsBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::map<std::string, std::unique_ptr<Bucket>> buckets;

};

}

#endif
