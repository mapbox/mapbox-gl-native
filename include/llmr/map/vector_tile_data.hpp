#ifndef LLMR_MAP_VECTOR_TILE_DATA
#define LLMR_MAP_VECTOR_TILE_DATA

#include <llmr/map/tile_data.hpp>


#include <llmr/renderer/bucket.hpp>

#include <llmr/geometry/vertex_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/icon_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/map/tile_parser.hpp>

#include <unordered_map>

namespace llmr {


class VectorTileData : public TileData {
    friend class TileParser;

public:
    VectorTileData(Tile::ID id, Map &map, const std::string url);
    ~VectorTileData();

    virtual void beforeParse();
    virtual void parse();
    virtual void afterParse();
    virtual void render(Painter &painter, const LayerDescription& layer_desc);
    virtual bool hasData(const LayerDescription& layer_desc) const;

protected:
    // Holds the actual geometries in this tile.
    FillVertexBuffer fillVertexBuffer;
    LineVertexBuffer lineVertexBuffer;
    IconVertexBuffer iconVertexBuffer;
    TextVertexBuffer textVertexBuffer;

    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;
    PointElementsBuffer pointElementsBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    std::unique_ptr<TileParser> parser;
};

}

#endif
